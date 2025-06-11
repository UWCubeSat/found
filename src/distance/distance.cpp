#include <cmath>
#include <utility>

#include "common/spatial/attitude-utils.hpp"
#include "common/spatial/camera.hpp"
#include "common/style.hpp"

#include "distance/distance.hpp"

namespace found {

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
    Vec3 mid1 = midpoint(spats[0], spats[1]);
    Vec3 mid2 = midpoint(spats[1], spats[2]);

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
    return radius_*sqrt(r * r + c * c)/r;
}

}  // namespace found
