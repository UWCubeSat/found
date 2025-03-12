#include <gtest/gtest.h>

#include <iostream>
#include <iomanip>
#include <cmath>

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


// Radius of Earth (m)
#define RADIUS_OF_EARTH 6378000.0
// Default DoubleEquals Tolerance (So big because of floating point problems)
#define DEFAULT_TOLERANCE 0.01


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

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthX1) {
    // Step I: Pick some distance (m) and a Camera
    decimal x_E = RADIUS_OF_EARTH + 1000000;
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 1, imageWidth, imageHeight);  // Focal length of 12 m
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

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {cam.SpatialToCamera(p1),
                cam.SpatialToCamera(p2),
                cam.SpatialToCamera(p3)};

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);

    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthX2) {
    // Step I: Pick some distance (m) and a Camera
    decimal x_E = RADIUS_OF_EARTH + 10000000;
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {x_E, 0, 0};

    // Step II: Figure out my projection points

    // a) Find the angle
    decimal alpha = asin(RADIUS_OF_EARTH / x_E);

    // b) Find the distance away from each projection point
    decimal p = sqrt(x_E * x_E - RADIUS_OF_EARTH * RADIUS_OF_EARTH);
    decimal centerMag = static_cast<decimal>(p * cos(alpha));
    decimal projectionRadiusMag = static_cast<decimal>(p * sin(alpha));

    // c) Use 3 easy projections
    Vec3 p1 = {centerMag, projectionRadiusMag * -std::cos(0.1), projectionRadiusMag * std::sin(0.1)};
    Vec3 p2 = {centerMag, projectionRadiusMag, 0};
    Vec3 p3 = {centerMag, projectionRadiusMag * -std::cos(-0.1), projectionRadiusMag * std::sin(-0.1)};

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {cam.SpatialToCamera(p1),
                cam.SpatialToCamera(p2),
                cam.SpatialToCamera(p3)};

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);

    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthY1) {
    // Step I: Pick some distance (m) and a Camera
    decimal x_E = 70000000;
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {0, x_E, 0};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {{ (decimal) 164.5803738848234729630348738282918930053710937500000000000000000000, (decimal) 991.8012832451947815570747479796409606933593750000000000000000000000},
                    {(decimal) 226.4719365675742324128805194050073623657226562500000000000000000000, (decimal) 708.6034128081986409597448073327541351318359375000000000000000000000},
                    {(decimal) 232.4013901714329222158994525671005249023437500000000000000000000000,(decimal) 362.7771025605316594919713679701089859008789062500000000000000000000}};

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);
    printf("%f, %f, %f \n", actual.x, actual.y, actual.z);
    
    printf("%f \n", actual.Magnitude());

    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthY2) {
    // Step I: Pick some distance (m) and a Camera
    decimal x_E = 70000000;
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {0, x_E, 0};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {{ (decimal) 271.5639315821267700812313705682754516601562500000000000000000000000, (decimal) 24.7609584897558789862159756012260913848876953125000000000000000000},
                    {(decimal) 355.6295230493384451619931496679782867431640625000000000000000000000, (decimal) 508.3634641727322218685003463178873062133789062500000000000000000000},
                    {(decimal) 308.3699606216349593523773364722728729248046875000000000000000000000, (decimal) 324.0180483597874854240217246115207672119140625000000000000000000000}};

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);
    //actual = actual * (decimal) 0.00002;
    printf("%f, %f, %f \n", actual.x, actual.y, actual.z);

    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

// TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthY1) {
//     //Step 0: Determine Quaterion rotation
//     found::Quaternion positionDirection = found::SphericalToQuaternion(static_cast<decimal> (3 * M_PI / 2), static_cast<decimal>(0), static_cast<decimal>(0));

//     // Step I: Pick some distance (m) and a Camera
//     decimal y_E = RADIUS_OF_EARTH + 10000000;
//     int imageWidth = 1024;
//     int imageHeight = 1024;
//     Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
//     PositionVector expected = {0, y_E, 0};

//     // Step II: Figure out my projection points

//     // a) Find the angle
//     decimal alpha = asin(RADIUS_OF_EARTH / y_E);

//     // b) Find the distance away from each projection point
//     decimal p = sqrt(y_E * y_E - RADIUS_OF_EARTH * RADIUS_OF_EARTH);
//     decimal centerMag = static_cast<decimal>(p * cos(alpha));
//     decimal projectionRadiusMag = static_cast<decimal>(p * sin(alpha));

//     // c) Use 3 easy projections
//     Vec3 p1 = {centerMag, projectionRadiusMag * -std::cos(0.1), projectionRadiusMag * std::sin(0.1)};
//     Vec3 p2 = {centerMag, projectionRadiusMag, 0};
//     Vec3 p3 = {centerMag, projectionRadiusMag * -std::cos(-0.1), projectionRadiusMag * std::sin(-0.1)};

//     Vec3 p1Rotated = positionDirection.Rotate(p1);
//     Vec3 p2Rotated = positionDirection.Rotate(p2);
//     Vec3 p3Rotated = positionDirection.Rotate(p3);


//     // Step III: Use CTS to convert to 2D vectors
//     Points pts = {cam.SpatialToCamera(p1Rotated),
//                 cam.SpatialToCamera(p2Rotated),
//                 cam.SpatialToCamera(p3Rotated)};

//     // Step IV: Run It and Test!
//     SphericalDistanceDeterminationAlgorithm algo =
//         SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

//     PositionVector actual = algo.Run(pts);

//     VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
// }
