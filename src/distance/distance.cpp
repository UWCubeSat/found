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
    if (p.size() < 3) return Vec3(0, 0, 0);

    const Vec3 spats[3] = {cam_.PixelToImageCoordinates(p[0]).normalized(),
                      cam_.PixelToImageCoordinates(p[p.size() / 2]).normalized(),
                      cam_.PixelToImageCoordinates(p[p.size() - 1]).normalized()};

    return this->Run(spats[0], spats[1], spats[2]);
}

PositionVector SphericalDistanceDeterminationAlgorithm::Run(const Vec3 &a, const Vec3 &b, const Vec3 &c) {
    // Obtain the center point of the projected circle
    this->center_ = getCenter(a, b, c);

    // Obtain the radius of the projected circle
    this->r_ = Distance(a, this->center_);

    // Obtain the distance from earth
    decimal h = this->radius_/this->r_;

    // You have to normalize the center vector here
    return this->center_.normalized() * h;
}

Vec3 SphericalDistanceDeterminationAlgorithm::getCenter(const Vec3 &a, const Vec3 &b, const Vec3 &c) {
    Vec3 diff1 = b - a;
    Vec3 diff2 = c - b;

    // Cross product to find the normal vector for points on earth
    Vec3 circleN = diff1.cross(diff2);
    Vec3 circlePt = a;

    // Mid point between 2 vectors
    Vec3 mid1 = Midpoint(a, b);
    Vec3 mid2 = Midpoint(b, c);

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
    matrix << circleN.x(), circleN.y(), circleN.z(),
              mid1N.x(), mid1N.y(), mid1N.z(),
              mid2N.x(), mid2N.y(), mid2N.z();

    decimal alpha = circleN.dot(circlePt);
    decimal beta = mid1N.dot(mid1);
    decimal gamma = mid2N.dot(mid2);

    Vec3 y(alpha, beta, gamma);

    Vec3 center = matrix.inverse() * y;

    return center;
}

///// IterativeSphericalDistanceDeterminationAlgorithm /////

IterativeSphericalDistanceDeterminationAlgorithm::IterativeSphericalDistanceDeterminationAlgorithm(decimal radius,
                                                                                        Camera &&cam,
                                                                                        size_t minimumIterations,
                                                                                        size_t maximumRefreshes,
                                                                                        decimal distanceRatio,
                                                                                        decimal discriminatorRatio,
                                                                                        int pdfOrder,
                                                                                        int radiusLossOrder)
      : SphericalDistanceDeterminationAlgorithm(radius, std::forward<Camera>(cam)),
        minimumIterations_(minimumIterations),
        maximumRefreshes_(maximumRefreshes),
        distanceRatioSq_(distanceRatio * distanceRatio),
        discriminatorRatio_(discriminatorRatio) {
    if (pdfOrder < 2) pdfOrder = 2;
    if (radiusLossOrder < 2) radiusLossOrder = 2;
    this->maximumRefreshes_ = maximumRefreshes;
    this->pdfOrder_ = static_cast<uint64_t>(pdfOrder + pdfOrder % 2);
    this->radiusLossOrder_ = static_cast<uint64_t>(radiusLossOrder + radiusLossOrder % 2);

    assert(this->pdfOrder_ >= 2);
    assert(this->pdfOrder_ % 2 == 0);
    assert(this->radiusLossOrder_ >= 2);
    assert(this->radiusLossOrder_ % 2 == 0);
}

PositionVector IterativeSphericalDistanceDeterminationAlgorithm::Run(const Points &p) {
    // Step -1: Return zero if the number of points is less than 0
    if (p.size() < 3) return Vec3(0, 0, 0);

    // Step 0: Determine the number of iterations
    size_t numIterations = this->minimumIterations_ > p.size() / 3 ? this->minimumIterations_ : p.size();
    size_t refreshFrequency = numIterations / (
        (this->maximumRefreshes_ < numIterations / 2 ? this->maximumRefreshes_ : numIterations / 2 - 1)
        + 1);

    // Step 1a: Get all unit vector projections of each point and setup logits
    size_t i = 0;
    size_t pointsSize = p.size();
    std::unique_ptr<Vec3[]> projectedPoints(new Vec3[pointsSize]);
    for (const Vec2 &point : p) {
        projectedPoints[i++] = this->cam_.PixelToImageCoordinates(point).normalized();
    }
    i = 0;
    std::unique_ptr<uint64_t[]> logits(new uint64_t[pointsSize]);

    // Step 2a: Use the first estimate as a reference
    PositionVector first(SphericalDistanceDeterminationAlgorithm::Run(p));
    decimal targetDistSq = first.squaredNorm();
    decimal referenceLoss = this->GenerateLoss(first, targetDistSq, projectedPoints, pointsSize);
    // Step 2b: Setup the cumulative loss and position. We are using softmax, normalized on the reference
    decimal totalLoss = DECIMAL_EXP(-1.0);
    PositionVector totalPosition = first * totalLoss;

    // Step 3: Iterate through all triplets and run them through SDDA,
    // generating a softmax statistic on each
    while (i != numIterations) {
        // Step 3b: Get the position from SDDA
        PositionVector position(this->ShuffledCall(projectedPoints, pointsSize, logits));
        decimal loss = this->GenerateLoss(position, targetDistSq, projectedPoints, pointsSize) / referenceLoss;
        if (loss <= this->discriminatorRatio_) {
            decimal factor = DECIMAL_EXP(-loss);
            totalLoss += factor;
            totalPosition += position * factor;
            i++;
        }

        if (i % refreshFrequency == 0) {
            totalPosition = totalPosition / totalLoss;
            targetDistSq = totalPosition.squaredNorm();
            referenceLoss = this->GenerateLoss(totalPosition, targetDistSq, projectedPoints, pointsSize);
            totalLoss = DECIMAL_EXP(-1.0);
            totalPosition = totalPosition * totalLoss;
        }
    }

    // Step 4: Return the softmax of the composed algorithm via the random triplets
    return totalPosition / totalLoss;
}

decimal IterativeSphericalDistanceDeterminationAlgorithm::GenerateLoss(PositionVector &position,
                                                                       decimal targetDistanceSq,
                                                                       std::unique_ptr<Vec3[]> &projectedPoints,
                                                                       size_t size) {
    // Generate the loss on point (offset it so it won't be nan, and initialize with distance
    // error):
    decimal loss = DECIMAL(1e-3);
    // If the distance error is outside the ratio, then we add distance loss
    decimal distance_sq_loss_ratio = DECIMAL_ABS((targetDistanceSq - position.squaredNorm())) / targetDistanceSq;
    if (distance_sq_loss_ratio >= this->distanceRatioSq_) {
        loss += distance_sq_loss_ratio * targetDistanceSq;
    }
    // Now, we obtain the radius error
    decimal targetRadiusSq = this->r_ * this->r_;
    for (size_t k = 0; k < size; k++) {
        decimal radius_loss_k = targetRadiusSq - (this->center_ - projectedPoints[k]).squaredNorm();
        loss += DECIMAL_POW(radius_loss_k, this->radiusLossOrder_);
    }

    return loss;
}

PositionVector IterativeSphericalDistanceDeterminationAlgorithm::ShuffledCall(
                                    std::unique_ptr<Vec3[]> &source,
                                    size_t n,
                                    std::unique_ptr<uint64_t[]> &logits) {
    // Step 0: Setup the random number generators
    static std::random_device device;  // GCOVR_EXCL_LINE
    static std::mt19937 gen(device());  // GCOVR_EXCL_LINE
    // This is okay (being static) since we always override the values

    // Uniformly generate the first number
    std::uniform_int_distribution<size_t> dist(0, n - 1);
    size_t index1 = dist(gen);
    Vec3 &a = source[index1];
    assert(dist.min() == 0);
    assert(dist.max() == static_cast<size_t>(n - 1));

    // Create logits for the second (even polynomial centered at indicies[i - 1])
    for (uint64_t j = 0; j < n; j++) {
        logits[j] = this->Pow(j - index1, this->pdfOrder_);
    }
    // Sample for the next number
    std::discrete_distribution<size_t> dist1(logits.get(), logits.get() + n);
    size_t index2 = dist1(gen);
    Vec3 &b = source[index2];
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
    Vec3 &c = source[dist2(gen)];
    assert(dist2.min() == 0);
    assert(dist2.max() == n - 1);

    return this->SphericalDistanceDeterminationAlgorithm::Run(a, b, c);
}

}  // namespace found
