#include "distance/distance.hpp"

#include <assert.h>

#include <cmath>
#include <utility>
#include <random>
#include <memory>

#include "common/logging.hpp"
#include "common/spatial/attitude-utils.hpp"
#include "common/spatial/camera.hpp"
#include "common/style.hpp"

namespace found {

///// SphericalDistanceDeterminationAlgorithm /////

PositionVector SphericalDistanceDeterminationAlgorithm::Run(const Points &p) {
    if (p.size() < 3) return {0, 0, 0};

    const Vec3 spats[3] = {cam_.CameraToSpatial(p[0]).Normalize(),
                      cam_.CameraToSpatial(p[p.size() / 2]).Normalize(),
                      cam_.CameraToSpatial(p[p.size() - 1]).Normalize()};

    return this->Run(spats);
}

PositionVector SphericalDistanceDeterminationAlgorithm::Run(const Vec3 *p) {
    // Obtain the center point of the projected circle
    this->center_ = getCenter(p);

    // Obtain the radius of the projected circle
    this->r_ = Distance(p[0], this->center_);

    // Obtain the distance from earth
    decimal h = this->radius_/this->r_;

    // You have to normalize the center vector here
    return this->center_.Normalize() * h;
}

Vec3 SphericalDistanceDeterminationAlgorithm::getCenter(const Vec3 *spats) {
    Vec3 diff1 = spats[1] - spats[0];
    Vec3 diff2 = spats[2] - spats[1];

    // Cross product to find the normal vector for points on earth
    Vec3 circleN = diff1.CrossProduct(diff2);
    Vec3 circlePt = spats[0];

    // Mid point between 2 vectors
    Vec3 mid1 = Midpoint(spats[0], spats[1]);
    Vec3 mid2 = Midpoint(spats[1], spats[2]);

    Vec3 mid1N = diff1;
    Vec3 mid2N = diff2;

    /**
     * CirclePt is a vector that points to a point on the plane. We also know the center vector should point to a point on the plane.
     * So, we get (circlePt - center) * circleN = 0. This is equivalent to (center * circleN) = (circlePt * circleN)
     * 
     * We have (center - mid1/mid2) gives us the vector perpendicular to the mid1N/mid2N vector. Hence,
     * (center - mid1)*mid1N = 0. This becomes (mid1N * center) = (mid1N * mid1). (This is the same for mid2)
     * So we have:
     * circleN * center = circleN * circlePt
     * mid1N * center = mid1N * mid1
     * mid2N * center = mid2N * mid2
     * This becomes a systems of linear equation
     */
    Mat3 matrix;
    matrix = {circleN.x, circleN.y, circleN.z, mid1N.x, mid1N.y,
              mid1N.z, mid2N.x, mid2N.y, mid2N.z};

    decimal alpha = circleN*circlePt;
    decimal beta = mid1N*mid1;
    decimal gamma = mid2N*mid2;

    Vec3 y = {alpha, beta, gamma};

    Vec3 center = matrix.Inverse() * y;

    return center;
}

///// IterativeSphericalDistanceDeterminationAlgorithm /////

IterativeSphericalDistanceDeterminationAlgorithm::IterativeSphericalDistanceDeterminationAlgorithm(decimal radius,
                                                                                        Camera &&cam,
                                                                                        size_t minimumIterations,
                                                                                        decimal distanceRatio,
                                                                                        decimal discriminatorRatio,
                                                                                        int pdfOrder,
                                                                                        int radiusLossOrder)
      : SphericalDistanceDeterminationAlgorithm(radius, std::forward<Camera>(cam)),
        minimumIterations_(minimumIterations),
        distanceRatioSq_(distanceRatio * distanceRatio),
        discriminatorRatio_(discriminatorRatio) {
    if (pdfOrder < 2) pdfOrder = 2;
    if (radiusLossOrder < 2) radiusLossOrder = 2;
    this->pdfOrder_ = static_cast<uint64_t>(pdfOrder + pdfOrder % 2);
    this->radiusLossOrder_ = static_cast<uint64_t>(radiusLossOrder + radiusLossOrder % 2);

    assert(this->pdfOrder_ >= 2);
    assert(this->pdfOrder_ % 2 == 0);
    assert(this->radiusLossOrder_ >= 2);
    assert(this->radiusLossOrder_ % 2 == 0);
}

PositionVector IterativeSphericalDistanceDeterminationAlgorithm::Run(const Points &p) {
    // Step -1: Return zero if the number of points is less than 0
    if (p.size() < 3) return {0, 0, 0};

    // Step 0: Determine the number of iterations
    size_t numIterations = this->minimumIterations_ > p.size() / 3 ? this->minimumIterations_ : p.size();

    // Step 1a: Get all unit vector projections of each point
    size_t i = 0;
    size_t j = 0;
    size_t pointsSize = p.size();
    std::unique_ptr<Vec3[]> projectedPoints(new Vec3[pointsSize]);
    for (const Vec2 &point : p) {
        projectedPoints[i++] = this->cam_.CameraToSpatial(point).Normalize();
    }
    i = 0;
    // Step 1b: Setup random triplets to use for SDDA
    size_t indicies_size = numIterations * 3;
    std::unique_ptr<Vec3[]> indicies(new Vec3[indicies_size]);
    this->Shuffle(indicies_size, pointsSize, projectedPoints, indicies);

    // Step 2a: Use the first estimate as a reference
    PositionVector first(SphericalDistanceDeterminationAlgorithm::Run(p));
    decimal targetRSq = (this->cam_.CameraToSpatial(p[0]).Normalize() - first.Normalize()).MagnitudeSq();
    decimal targetDistSq = first.MagnitudeSq();
    decimal initialLoss = this->GenerateLoss(first, targetDistSq, targetRSq, projectedPoints, pointsSize);
    // Step 2b: Setup the cumulative loss and position. We are using softmax, normalized on the reference
    decimal totalLoss = DECIMAL_EXP(-1.0);
    PositionVector totalPosition;
    totalPosition = first * totalLoss;

    // Step 3: Iterate through all triplets and run them through SDDA,
    // generating a softmax statistic on each
    while (i != numIterations) {
        // GCOVR_EXCL_START
        // Step 3a: Shuffle when we've passed our last triplet
        if (j >= indicies_size) {
            indicies_size = 3 * (numIterations - i);
            j = 0;
            this->Shuffle(indicies_size, pointsSize, projectedPoints, indicies);
        }
        // GCOVR_EXCL_STOP
        // Step 3b: Get the position from SDDA
        PositionVector position(SphericalDistanceDeterminationAlgorithm::Run(indicies.get() + j));
        targetRSq = this->r_ * this->r_;
        decimal loss = this->GenerateLoss(position, targetDistSq, targetRSq, projectedPoints, pointsSize) / initialLoss;
        if (loss <= this->discriminatorRatio_) {
            decimal factor = DECIMAL_EXP(-loss);
            totalLoss += factor;
            totalPosition += position * factor;
            i++;
        }

        j += 3;
    }

    // Step 4: Return the softmax of the composed algorithm via the random triplets
    return totalPosition / totalLoss;
}

decimal IterativeSphericalDistanceDeterminationAlgorithm::GenerateLoss(PositionVector &position,
                                                                       decimal targetDistanceSq,
                                                                       decimal targetRadiusSq,
                                                                       std::unique_ptr<Vec3[]> &projectedPoints,
                                                                       size_t size) {
    // Generate the loss on point (offset it so it won't be nan, and initialize with distance
    // error):
    decimal loss = DECIMAL(1e-3);
    // If the distance error is outside the ratio, then we add distance loss
    decimal distance_sq_loss_ratio = DECIMAL_ABS((targetDistanceSq - position.MagnitudeSq())) / targetDistanceSq;
    if (distance_sq_loss_ratio >= this->distanceRatioSq_) {
        loss += distance_sq_loss_ratio * targetDistanceSq;
    }
    // Now, we obtain the radius error
    for (size_t k = 0; k < size; k++) {
        decimal radius_loss_k = targetRadiusSq - (this->center_ - projectedPoints[k]).MagnitudeSq();
        loss += DECIMAL_POW(radius_loss_k, this->radiusLossOrder_);
    }

    return loss;
}

void IterativeSphericalDistanceDeterminationAlgorithm::Shuffle(size_t size,
                                                               size_t n,
                                                               std::unique_ptr<Vec3[]> &source,
                                                               std::unique_ptr<Vec3[]> &indicies) {
    static std::random_device device;  // GCOVR_EXCL_LINE
    static std::mt19937 gen(device());  // GCOVR_EXCL_LINE
    assert(size % 3 == 0);
    std::uniform_int_distribution<size_t> dist(0, n - 1);
    // Operates under the asusmption you won't call Run more than once
    std::unique_ptr<uint64_t[]> logits(new uint64_t[n]);
    size_t i = 0;
    while (i < size) {
        // Uniformly generate the first number
        size_t index1 = dist(gen);
        indicies[i++] = source[index1];
        assert(dist.min() == 0);
        assert(dist.max() == static_cast<size_t>(n - 1));

        // Create logits for the second (even polynomial centered at indicies[i - 1])
        for (uint64_t j = 0; j < n; j++) {
            logits[j] = this->Pow(j - index1, this->pdfOrder_);
        }
        // Sample for the next number
        std::discrete_distribution<size_t> dist1(logits.get(), logits.get() + n);
        size_t index2 = dist1(gen);
        indicies[i++] = source[index2];
        assert(dist1.min() == 0);
        assert(dist1.max() == n - 1);

        // Update the logits for the third number (bi-even polynomial with
        // centers at indicies[i - 1] and indicies[i - 2]). Note that
        // the function is zero at both our chosen indicies
        for (uint64_t j = 0; j < n; j++) {
            logits[j] *= this->Pow(j - index2, this->pdfOrder_);
        }
        // Sample for the last number
        std::discrete_distribution<size_t> dist2(logits.get(), logits.get() + n);
        indicies[i++] = source[dist2(gen)];
        assert(dist2.min() == 0);
        assert(dist2.max() == n - 1);
    }
}

}  // namespace found
