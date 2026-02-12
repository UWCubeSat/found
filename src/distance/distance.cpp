// TEMP
#include <string>
#include <sstream>
// TEMP

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
    if (p.size() < 3) return {0, 0, 0}; // If someone puts in less than 3 points, we're probably at earth's core

    const Mat3 DiagAxes = {principleAxes_.x,0,0,
                              0,principleAxes_.y,0,
                              0,0,principleAxes_.z};

    const Mat3 DiagInvAxes = {1/principleAxes_.x,0,0,
                              0,1/principleAxes_.y,0,
                              0,0,1/principleAxes_.z};


    Mat3 invCameraProjMat = ComputeInvCameraProjMat(cam_); 
    
    Mat3 TPC = ComputeBodyToCamTransformation(AOR_);
    
    Mat3 imageToSpace = DiagInvAxes * TPC.Transpose() * invCameraProjMat;

    int i = 0;
    size_t pointsSize = p.size();
    std::vector<Vec2> undistortedPoints = std::vector<Vec2>(pointsSize, {0.0,0.0});
    for (const Vec2 &point : p) {
        // Vec2 center = {cam_.XCenter(), cam_.YCenter()};
        undistortedPoints[i++] = point; //UndistortPoint(point, center, radialCoefficients_, tangentialCoefficients_);
    }
    i=0;
    std::vector<Vec3> normalizedVecsToHorizon = std::vector<Vec3>(pointsSize, {0.0, 0.0, 0.0});
    for (const Vec2 &point : p) {
        Vec3 pBar = {point.x, point.y, 1};
        normalizedVecsToHorizon[i++] = (imageToSpace * pBar).Normalize();
    }
    
    Vec3 vecToEarth = VecToEarthTLS(normalizedVecsToHorizon); // vecToEarth magnitude is a function of the distance and the principal axes
    vecToEarth = (TPC * DiagAxes * vecToEarth) * (1/sqrt(vecToEarth*vecToEarth - 1)); // I really want to use fastinvsqrt but that would probably send our satellite into the sun

    return vecToEarth;
}

Mat3 SpheroidDistanceDeterminationAlgorithm::ComputeBodyToCamTransformation(Vec3 AOR){
    Vec3 randomVec = {0.57735,0.57735,0.57735}; // pick a random vector
    Vec3 tempAOR = {AOR.x, AOR.y, -AOR.z};
    Vec3 orthogonalVec1 = tempAOR.CrossProduct(randomVec).Normalize(); // do a cross product to get a vector orthogonal to AOR, which will be on the equator
    Vec3 orthogonalVec2 = tempAOR.CrossProduct(orthogonalVec1).Normalize(); // get the other orthogonal vector on the equator; should already be normalized

    // just use basis vectors as columns
    // flip z since we're going from a right handed system to a left handed one
    Mat3 TPC = {
        orthogonalVec1.x,    orthogonalVec2.x,   AOR.x, 
        orthogonalVec1.y,    orthogonalVec2.y,   AOR.y, 
        -orthogonalVec1.z,   -orthogonalVec2.z,  AOR.z
    };

    return TPC;
}

// stupid equation
Vec2 SpheroidDistanceDeterminationAlgorithm::UndistortPoint(Vec2 point, Vec2 center, Vec3 k, Vec2 p){
    Vec2 vecFromCenter = point-center;
    decimal r = vecFromCenter.x*vecFromCenter.x + vecFromCenter.y*vecFromCenter.y;
    Vec2 term1 = vecFromCenter*(1+k.x*r*r+k.y*r*r*r*r+k.z*r*r*r*r*r*r);
    decimal x = vecFromCenter.x;
    decimal y= vecFromCenter.y;
    Vec2 term2 = {2*p.x*x*y + p.y*(r*r+2*x*x), p.y*(r*r*+2*y*y)+2*p.y*x*y};
    return term1+term2;
}

Mat3 SpheroidDistanceDeterminationAlgorithm::ComputeInvCameraProjMat(Camera cam){
    decimal dx = cam.FocalLength()/cam.PixelSize();
    decimal dy = dx;
    Mat3 KInv = {
        1/dx,   0,      -cam.XCenter()/dx,
        0,      1/dy,   -cam.YCenter()/dy,
        0,      0,      1
    };

    return KInv;
}

// Supposedly this is how to do TLS. I have no idea why.
Vec3 SpheroidDistanceDeterminationAlgorithm::VecToEarthTLS(std::vector<Vec3> &normalizedVecsToHorizon){
    int size = normalizedVecsToHorizon.size();
    Matrix A(size, 4);
    for (int i = 0; i < size; i++){
        A(i, 0) = normalizedVecsToHorizon[i].x;
        A(i, 1) = normalizedVecsToHorizon[i].y;
        A(i, 2) = normalizedVecsToHorizon[i].z;
        A(i, 3) = 1;
    }
    SVDResult result = ComputeSVD(A, 4);
    Matrix V = ComputeSVD(A, 4).V; 
    V = V.Transpose();
    Vec3 x = {V.Get(V.NumCols()-1,0), V.Get(V.NumCols()-1,1), V.Get(V.NumCols()-1,2)};
    x = x/V.Get(V.NumRows()-1, V.NumCols()-1);
    return x;    
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
