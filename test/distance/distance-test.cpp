#include <gtest/gtest.h>

#include <cmath>
#include <iostream>
#include <iomanip>

#include "src/style/style.hpp"
#include "src/spatial/attitude-utils.hpp"
#include "src/spatial/camera.hpp"
#include "src/distance/distance.hpp"


/* Using Directives */
using found::Camera;
using found::Vec3;
using found::Points;
using found::PositionVector;
using found::decimal;
using found::SphericalDistanceDeterminationAlgorithm;


/* Common Constants */


// Radius of Earth (km)
#define RADIUS_OF_EARTH 6378.0
// Default DoubleEquals Tolerance (So big because of floating point problems)
#define DEFAULT_TOLERANCE 1


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
    EXPECT_LT(abs(vec1.x - vec2.x), tolerance); \
    EXPECT_LT(abs(vec1.y - vec2.y), tolerance); \
    EXPECT_LT(abs(vec1.z - vec2.z), tolerance);

std::ostream &operator<<(std::ostream &stream, const Vec3 &vector) {
    stream << std::fixed << std::setprecision(5) << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
    return stream;
}

// Base Case I: The image captured contains an edge centered about the image

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarth) {
    // Step I: Pick some distance (km) and a Camera
    decimal x_E = RADIUS_OF_EARTH + 1000;
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, imageWidth, imageHeight);  // Focal length of 12 m
    PositionVector expected = {x_E, 0, 0};

    // Step II: Figure out my projection points

    // a) Find the angle
    decimal alpha = asin(RADIUS_OF_EARTH / x_E);

    // b) Find the distance away from each projection point
    decimal p = sqrt(x_E * x_E - RADIUS_OF_EARTH * RADIUS_OF_EARTH);

    // c) Use 3 easy projections
    Vec3 p1 = {static_cast<decimal>(p * cos(alpha)), static_cast<decimal>(p * sin(alpha)), 0};
    Vec3 p2 = {static_cast<decimal>(p * cos(alpha)), static_cast<decimal>(-p * sin(alpha)), 0};
    Vec3 p3 = {static_cast<decimal>(p * cos(alpha)), 0, static_cast<decimal>(p * sin(alpha))};

    std::cout << p1 * (1/p1.x) << std::endl;

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {cam.SpatialToCamera(p1),
                cam.SpatialToCamera(p2),
                cam.SpatialToCamera(p3)};

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);

    std::cout << "Actual Result: " << actual << std::endl;

    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}
