#include <math.h>

#include "attitude-utils.hpp"
#include "style.hpp"
#include "camera.hpp"

#include "distance.hpp"

#define RADIUS_OF_EARTH 6378.0

namespace found {  

distFromEarth SphericalDistanceDeterminationAlgorithm::Run(char *image, Points &p, int imageWidth, int imageHeight)
{
    return solve(p, radius_);
}

Vec3 SphericalDistanceDeterminationAlgorithm::getCenter(Vec3* spats){
    Vec3 diff1 = spats[1] - spats[0];
    Vec3 diff2 = spats[2] -spats[1];

    Vec3 circleN = diff1.CrossProduct(diff2);
    Vec3 circlePt = spats[0];

    Vec3 mid1 = midpoint(spats[0], spats[1]);
    Vec3 mid2 = midpoint(spats[1], spats[2]);

    Vec3 mid1N = diff1;
    Vec3 mid2N = diff2;

    Mat3 matrix;
    matrix = {circleN.x, circleN.y, circleN.z, mid1N.x, mid1N.y, mid1N.z, mid2N.x, mid2N.y, mid2N.z};

    int alpha = circleN*circlePt;
    int beta = mid1N*mid1;
    int gamma = mid2N*mid2;

    Vec3 y = {alpha, beta, gamma};

    Vec3 center = matrix.Inverse() * y;

    return center;
}

decimal SphericalDistanceDeterminationAlgorithm::getRadius(Vec3* spats, Vec3 center){
    return Distance(spats[0], center);
}

decimal SphericalDistanceDeterminationAlgorithm::getDistance(decimal r)
{
    return RADIUS_OF_EARTH/r;
}

distFromEarth SphericalDistanceDeterminationAlgorithm::solve(Points& pts, int R){
    Vec3 uSpats[3] = {cam_.CameraToSpatial(pts[0]).Normalize(), cam_.CameraToSpatial(pts[1]).Normalize(), cam_.CameraToSpatial(pts[2]).Normalize()};

    Vec3 center = getCenter(uSpats);
    decimal r = getRadius(uSpats, center);

    decimal h = getDistance(r);

    return center * h;
}
};
