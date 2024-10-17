#include <math.h>
#include <utility>
#include <iomanip>

#include "spatial/attitude-utils.hpp"
#include "spatial/camera.hpp"
#include "style/style.hpp"

#include "distance/distance.hpp"

namespace found {

PositionVector SphericalDistanceDeterminationAlgorithm::Run(const Points &p) {
    // We do not normalize here, because the assumption for getting the distance
    // is dependent on the fact that vec.x is 1. It is also not cost effective
    // as the tradeoff is to nromalize center instead (3 norms v. 1 norm)
    Vec3 spats[3] = {cam_.CameraToSpatial(p[0]),
                      cam_.CameraToSpatial(p[1]),
                      cam_.CameraToSpatial(p[2])};

    // Obtain the center point of the projected circle
    Vec3 center = std::move(getCenter(spats));

    // Obtain the radius of the projected circle
    decimal r = getRadius(spats, center);

    // Obtain the distance from earth
    decimal h = getDistance(r);

    // You have to normalize the center vector here
    return center.Normalize() * h;
}

Vec3 SphericalDistanceDeterminationAlgorithm::getCenter(Vec3 spats[3]) {
    Vec3 diff1 = std::move(spats[1] - spats[0]);
    Vec3 diff2 = std::move(spats[2] - spats[1]);

    Vec3 circleN = std::move(diff1.CrossProduct(diff2));
    Vec3 circlePt = spats[0];

    Vec3 mid1 = std::move(midpoint(spats[0], spats[1]));
    Vec3 mid2 = std::move(midpoint(spats[1], spats[2]));

    Vec3 mid1N = std::move(diff1);
    Vec3 mid2N = std::move(diff2);

    Mat3 matrix;
    matrix = {circleN.x, circleN.y, circleN.z, mid1N.x, mid1N.y,
              mid1N.z, mid2N.x, mid2N.y, mid2N.z};

    decimal alpha = circleN*circlePt;
    decimal beta = mid1N*mid1;
    decimal gamma = mid2N*mid2;

    Vec3 y = {alpha, beta, gamma};

    Vec3 center = std::move(matrix.Inverse() * y);

    return center;
}

decimal SphericalDistanceDeterminationAlgorithm::getRadius(Vec3* spats,
Vec3 center) {
    return Distance(spats[0], center);
}

decimal SphericalDistanceDeterminationAlgorithm::getDistance(decimal r) {
    return radius_*sqrt(r * r + 1)/r;
}

}  // namespace found
