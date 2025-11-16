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

///// SpheroidDistanceDeterminationAlgorithm //////

PositionVector SpheroidDistanceDeterminationAlgorithm::Run(const Points &p) {
    if (conicSection_ == new Mat3(0,0,0,0,0,0,0,0,0)){ // check if conicSection_ was not initialized on construction
        if (p.size() < 3) return {0, 0, 0};
        conicSection_ = GetConicSection(p);
    }

    // We use the adjugate of the matrix, which describes the conic envelope (set of lines that are tangent to the conic)
    // This matrix is symmetric
    Mat3 conicEnvelope = conicSection_.Adjugate();
    // Make sure the determinant is negative so that only one of the eigenvalues is negative
    if (conicEnvelope.Det > ) conicEnvelope = -1 * conicEnvelope;

    Vec3 conicEnvelopeEigenvalues = conicEnvelope.EigenvaluesSymmetric();
    Mat3 conicEnvelopeEigenvectors = conicEnvelope.EigenvectorsSymmetric(conicEnvelopeEigenvalues);

    Mat3 possibleSolutions = SolveForPossiblePositions(principleAxisDimensions_, conicEnvelopeEigenvalues, conicEnvelopeEigenvectors);

    return pickPosition(possibleSolutions, principleAxisDimensions_.z, conicEnvelope);
}

Mat3 SpheroidDistanceDeterminationAlgorithm::SolveForPossiblePositions(Vec3 principleAxes, Vec3 conicEnvelopeEigenvalues, Mat3 conicEnvelopeEigenvectors){
    decimal a = principleAxes.x;
    decimal b = principleAxes.y;
    decimal c = principleAxes.z;

    // lambda1, lambda2, lambda3
    decimal l1 = conicEnvelopeEigenvalues.x;
    decimal l2 = conicEnvelopeEigenvalues.y;
    decimal l3 = conicEnvelopeEigenvalues.z;

    /**
    *  2D vector that can be manipulated to extract 2 possible solutions
    *  The solutions take the form of [eigenvector matrix] * (column vector)[0, +-sqrt(rho1), +-sqrt(rho2)]
    *  This produces four results but 2 will produce an image with Earth behind the camera (z < 0) so we can toss them
    *  The form is a bit esoteric but I don't think there's a way to intuitively understand it
    *  See tutorial paper (search tutorial in slack; Eqs. 205-207) for more details
    */
    decimal scale = (a*a*l1)/(l2 - l3);
    Vec2 rho = scale * Vec2(
        -((c/a)*(c/a) - l2/l1)*(1 - l2/l1),
        ((c/a)*(c/a) - l3/l1)*(1 - l3/l1)
    );

    Vec3 solution1 = conicEnvelopeEigenvectors * Vec3(0, sqrt(rho.x), sqrt(rho.y));  // there may be a way to compute these two operations faster?
    Vec3 solution2 = conicEnvelopeEigenvectors * Vec3(0, -sqrt(rho.x), sqrt(rho.y)); // they are almost the exact same

    // multiply by the sign of z to negate the vector if it places Earth behind the camera
    solution1 = solution1 * (int)((0 > solution1.z) - (solution1.z > 0));
    solution2 = solution2 * (int)((0 > solution2.z) - (solution2.z > 0));

    return Mat3(
        solution1.x, solution2.x, 0,
        solution1.y, solution2.y, 0,
        solution1.z, solution2.z, 0
    );
}

Vec3 SpheroidDistanceDeterminationAlgorithm::pickPosition(Mat3 possibleSolutions, decimal principleAxisC, Mat3 conicEnvelope){
    // Normalize to make calculations later easier
    Vec3 trueAxisOfRotation = cam.SpatialToCamera({0,0,1}).Normalize();
    // Renaming to make it clearer; If this causes performance issues just replace it
    decimal trueEigenvalue = principleAxisC*principleAxisC;

    // If the solution is correct, the axis of rotation will be (nearly) an eigenvector of the matrix formed 
    // by adding the conicEnvelope and the outerproduct of the solution 
    // see tutorial (Eqs. 168, 154)
    Vec3 possibleSolution1 =  possibleSolutions.column(0);
    decimal sol1EigenvectorCandidate = (conicEnvelope + possibleSolution1.OuterProduct(possibleSolution1))*trueAxisOfRotation;

    Vec3 possibleSolution2 =  possibleSolutions.column(1);
    decimal sol2EigenvectorCandidate = (conicEnvelope + possibleSolution2.OuterProduct(possibleSolution2))*trueAxisOfRotation;
    
    // Check how aligned the vectors are; if the a.o.r. was a true eigenvector, taking the dot product with the result of
    // the multiplication and dividing by the eigenvalue should give exactly 1
    decimal sol1Error = (sol1EigenvectorCandidate*trueAxisOfRotation / trueEigenvalue) - 1;
    decimal sol2Error = (sol2EigenvectorCandidate*trueAxisOfRotation / trueEigenvalue) - 1;
    
    // return the solution with less error; ideally it should not even be close
    return possibleSolution1 : possibleSolution2 ? DECIMAL_ABS(sol1Error) < DECIMAL_ABS(sol2Error);
}

///// SphericalDistanceDeterminationAlgorithm /////

PositionVector SphericalDistanceDeterminationAlgorithm::Run(const Points &p) {
    if (p.size() < 3) return {0, 0, 0};

    Vec3 spats[3] = {cam_.CameraToSpatial(p[0]).Normalize(),
                      cam_.CameraToSpatial(p[p.size() / 2]).Normalize(),
                      cam_.CameraToSpatial(p[p.size() - 1]).Normalize()};

    // Obtain the center point of the projected circle
    Vec3 center = getCenter(spats);

    // Obtain the radius of the projected circle
    PreciseDecimal r = getRadius(spats, center);

    // Obtain the distance from earth
    PreciseDecimal h = getDistance(r, center.Magnitude());

    // You have to normalize the center vector here
    return center.Normalize() * h;
}

Vec3 SphericalDistanceDeterminationAlgorithm::getCenter(Vec3 spats[3]) {
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

PreciseDecimal SphericalDistanceDeterminationAlgorithm::getRadius(Vec3* spats,
Vec3 center) {
    return Distance(spats[0], center);
}

PreciseDecimal SphericalDistanceDeterminationAlgorithm::getDistance(PreciseDecimal r, PreciseDecimal c) {
    return static_cast<PreciseDecimal>(radius_)*sqrt(r * r + c * c)/r;
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
    // Return zero if the number of points is less than 0
    if (p.size() < 3) return {0, 0, 0};

    // Determine the number of iterations
    size_t numIterations = this->minimumIterations_ > p.size() / 3 ? this->minimumIterations_ : p.size();

    // Setup to iterate through all the calls
    size_t i = 0;
    size_t j = 0;
    size_t pointsSize = p.size();
    std::unique_ptr<Vec3[]> projectedPoints(new Vec3[pointsSize]);
    for (const Vec2 &point : p) {
        projectedPoints[i++] = this->cam_.CameraToSpatial(point).Normalize();
    }
    i = 0;
    std::unique_ptr<decimal[]> losses(new decimal[numIterations]{});
    std::unique_ptr<PositionVector[]> positions(new PositionVector[numIterations]);
    PositionVector result{0, 0, 0};

    size_t indicies_size = numIterations * 3;
    std::unique_ptr<size_t[]> indicies(new size_t[indicies_size]);

    // Get the first estimate, and use it as the reference
    PositionVector first(SphericalDistanceDeterminationAlgorithm::Run(p));
    decimal targetRSq = (this->cam_.CameraToSpatial(p[0]).Normalize() - first.Normalize()).MagnitudeSq();
    decimal targetDistSq = first.MagnitudeSq();
    losses[i] = this->GenerateLoss(first, targetDistSq, targetRSq, projectedPoints, pointsSize);
    positions[i++] = first;

    // Initial shuffle
    this->Shuffle(indicies_size, pointsSize, indicies);

    // Iterate through all points, shuffling them into triplets to feed into
    // SphericalDistanceDeterminationAlgorithm::Run
    while (i != numIterations) {
        // Shuffle when we've passed our last triplet
        // GCOVR_EXCL_START
        if (j >= indicies_size) {
            indicies_size = 3 * (numIterations - i);
            j = 0;
            this->Shuffle(indicies_size, pointsSize, indicies);
        }
        // GCOVR_EXCL_STOP
        PositionVector position(
            SphericalDistanceDeterminationAlgorithm::Run({p[indicies[j]], p[indicies[j + 1]], p[indicies[j + 2]]}));
        if (!std::isnan(position.MagnitudeSq())) {  // GCOVR_EXCL_LINE
            // Only recalculate targetRSq, we want to keep our targetDistSq from earlier
            targetRSq = (this->cam_.CameraToSpatial(p[indicies[j]]).Normalize() - position.Normalize()).MagnitudeSq();
            losses[i] = this->GenerateLoss(position, targetDistSq, targetRSq, projectedPoints, pointsSize) / losses[0];
            if (losses[i] <= this->discriminatorRatio_) positions[i++] = position;
        }

        j += 3;
    }

    // Normalize losses[0]
    losses[0] = 1.0;

    // Now, perform softmax, which is sum(value * e^(-loss)) / sum(e^(-loss_i))
    decimal sum = 0;
    for (i = 0; i < numIterations; i++) {
        decimal factor = DECIMAL_EXP(-losses[i]);
        result += positions[i] * factor;
        sum += factor;
    }

    return result / sum;
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
    PositionVector positionNorm(position.Normalize());
    for (size_t k = 0; k < size; k++) {
        decimal radius_loss_k = targetRadiusSq - (positionNorm - projectedPoints[k]).MagnitudeSq();
        loss += DECIMAL_POW(radius_loss_k, this->radiusLossOrder_);
    }

    return loss;
}

void IterativeSphericalDistanceDeterminationAlgorithm::Shuffle(size_t size,
                                                               size_t n,
                                                               std::unique_ptr<size_t[]> &indicies) {
    static std::random_device device;  // GCOVR_EXCL_LINE
    static std::mt19937 gen(device());  // GCOVR_EXCL_LINE
    assert(size % 3 == 0);
    std::uniform_int_distribution<size_t> dist(0, n - 1);
    // Operates under the asusmption you won't call Run more than once
    std::unique_ptr<uint64_t[]> logits(new uint64_t[n]);
    size_t i = 0;
    while (i < size) {
        // Uniformly generate the first number
        indicies[i++] = dist(gen);
        assert(dist.min() == 0);
        assert(dist.max() == static_cast<size_t>(n - 1));

        // Create logits for the second (even polynomial centered at indicies[i - 1])
        for (uint64_t j = 0; j < n; j++) {
            logits[j] = this->Pow(j - indicies[i - 1], this->pdfOrder_);
        }
        // Sample for the next number
        std::discrete_distribution<size_t> dist1(logits.get(), logits.get() + n);
        indicies[i++] = dist1(gen);
        assert(dist1.min() == 0);
        assert(dist1.max() == n - 1);

        // Update the logits for the third number (bi-even polynomial with
        // centers at indicies[i - 1] and indicies[i - 2]). Note that
        // the function is zero at both our chosen indicies
        for (uint64_t j = 0; j < n; j++) {
            logits[j] *= this->Pow(j - indicies[i - 1], this->pdfOrder_);
        }
        // Sample for the last number
        std::discrete_distribution<size_t> dist2(logits.get(), logits.get() + n);
        indicies[i++] = dist2(gen);
        assert(dist2.min() == 0);
        assert(dist2.max() == n - 1);
    }
    #ifndef NDEBUG
        for (i = 0; i < size; i++) {
            assert(indicies[i] < n);
        }
    #endif
}

}  // namespace found
