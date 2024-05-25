
#include <catch.hpp>
#include <cmath>
#include <iostream>

#include "style.hpp"
#include "attitude-utils.hpp"
#include "camera.hpp"

#include "distance.hpp"


/* Using Directives */
using found::Camera;
using found::Vec3;
using found::Points;
using found::distFromEarth;
using found::decimal;
using found::SphericalDistanceDeterminationAlgorithm;


/* Common Constants */


// Radius of Earth (km)
#define RADIUS_OF_EARTH 6378.0
// Default DoubleEquals Tolerance
#define DEFAULT_TOLERANCE 1e-6


/* Test Macros */

/**
 * Requires that vec1 == vec2 (using DecimalEquals)
 * 
 * @param vec1 A Vec3 object
 * @param vec2 A Vec3 object
 * @param tolerance The tolerance for vec1 to be
 * "equal" to vec2
 * 
 * @post Will have REQUIRE'd that vec1 is equal to
 * vec2, on a component basis, within tolerance
*/
#define VECTOR_EQUALS(vec1, vec2, tolerance) \
    REQUIRE(DecimalEquals(vec1.x, vec2.x, tolerance)); \
    REQUIRE(DecimalEquals(vec1.y, vec2.y, tolerance)); \
    REQUIRE(DecimalEquals(vec1.z, vec2.z, tolerance));

/**
 * Compares 2 decimals to make sure they are "equal"
 * 
 * @param x1 The first decimal to compare
 * @param x2 The second decimal to compare
 * @param tolerance The maximum allowable error between the two
 * 
 * @return true iff x1 and x2 are the same within tolerance,
 * false otherwise
*/
bool DecimalEquals(decimal x1, decimal x2, decimal tolerance) {
    return abs(x1 - x2) < tolerance;
}

std::ostream &operator<<(std::ostream &stream, const Vec3 &vector) {
    stream << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
    return stream;
}

// Base Case I: The image captured contains an edge centered about the image

TEST_CASE("Base Case I: Entire Circle I") {
    // Step I: Pick some distance (km) and a Camera
    decimal x_E = RADIUS_OF_EARTH + 1000;
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, imageWidth, imageHeight);
    distFromEarth expected = {x_E, 0, 0};

    // Step II: Figure out my projection points

    // a) Find the angle
    decimal alpha = asin(RADIUS_OF_EARTH / x_E);

    // b) Find the distance away from each projection point
    decimal p = sqrt(x_E * x_E - RADIUS_OF_EARTH * RADIUS_OF_EARTH);

    // c) Use 3 easy projections
    Vec3 p1 = {p * cos(alpha), p * sin(alpha), 0};
    Vec3 p2 = {p * cos(alpha), -p * sin(alpha), 0};
    Vec3 p3 = {p * cos(alpha), 0, p * sin(alpha)};

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {cam.SpatialToCamera(p1),
                cam.SpatialToCamera(p2),
                cam.SpatialToCamera(p3)};

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);
    
    distFromEarth actual = algo.Run(nullptr, pts, imageWidth, imageHeight);

    std::cout << "Actual Result: " << actual << std::endl;

    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}
