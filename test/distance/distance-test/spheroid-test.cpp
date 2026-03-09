#include <gtest/gtest.h>

#include <cmath>
#include <utility>
#include <string>

#include "common/style.hpp"
#include "common/spatial/attitude-utils.hpp"
#include "common/spatial/camera.hpp"
#include "distance/distance.hpp"
#include "common/logging.hpp"
#include "common/decimal.hpp"
#include "calibrate/calibrate.hpp"
#include "test/common/common.hpp"

using found::Camera;
using found::Vec3;
using found::Vec2;
using found::Mat3;
using found::Points;
using found::PositionVector;
using found::SpheroidDistanceDeterminationAlgorithm;
using found::Quaternion;
using found::SphericalToQuaternion;
using found::DegToRad;
using found::EulerAngles;
using found::LOSTCalibrationAlgorithm;

// Default DoubleEquals Tolerance (So big for float because of floating point problems)
#ifdef FOUND_FLOAT_MODE
    #define DEFAULT_TOLERANCE DECIMAL(2000.0)
#endif
// The equatorial radius of Earth (m)
#define RADIUS_OF_EARTH_A (DECIMAL(6378.1366))
// The polar radius of Earth (m)
#define RADIUS_OF_EARTH_C (DECIMAL(6356.7519))

TEST(SpheroidDistanceDeterminationAlgorithmTest, TestNotEnoughPoints) {
     Points edge_pts = {
        {DECIMAL(611.7877295393952),  DECIMAL(264.09276117927675)},
        {DECIMAL(573.8922119671513),  DECIMAL(325.3968742337466)}
     };

    Camera cam(0.05, .0000514, 4,4);
    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), {1,1,1}, Quaternion::Identity());
    PositionVector actual = algo.Run(edge_pts);

    PositionVector expected = {0, 0, 0};
    ASSERT_VEC2_EQ(actual, expected, DEFAULT_TOLERANCE);
}
// TEST(SpheroidDistanceDeterminationAlgorithmTest, TestRandomEarth1) {
//     Vec3 principleAxisDimensions(RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_C);
//     int imageWidth = 700;
//     int imageHeight = 600;
//     Camera cam(0.05, .0000514, imageWidth, imageHeight);
//     Quaternion attitude = SphericalToQuaternion(DegToRad(180-36.3387), DegToRad(90-151.885), DegToRad(279.474));
//     Points edge_pts = {
//         {DECIMAL(611.7877295393952),  DECIMAL(264.09276117927675)},
//         {DECIMAL(573.8922119671513),  DECIMAL(325.3968742337466)},
//         {DECIMAL(350.4939091109065),  DECIMAL(589.3274589318719)},
//         {DECIMAL(391.14346133188616), DECIMAL(550.233909535328)},
//         {DECIMAL(464.0968679643527),  DECIMAL(471.5734768209798)},
//         {DECIMAL(454.4893660268322),  DECIMAL(482.63970990423604)},
//         {DECIMAL(658.5589230687222),  DECIMAL(177.48386735439033)},
//         {DECIMAL(373.4109900481945),  DECIMAL(567.6612715009444)},
//         {DECIMAL(684.6437823193647),  DECIMAL(122.43996936965047)},
//         {DECIMAL(479.9742150253835),  DECIMAL(452.75848062170667)},
//         {DECIMAL(518.2721763857571),  DECIMAL(404.41735985920894)},
//         {DECIMAL(491.85561942267265), DECIMAL(438.2265392379015)},
//         {DECIMAL(639.7657274333935),  DECIMAL(213.9521877859907)},
//         {DECIMAL(694.1329604135699),  DECIMAL(100.94348878568302)},
//         {DECIMAL(571.5349509252572),  DECIMAL(328.99120144536215)},
//         {DECIMAL(379.6326629337276),  DECIMAL(561.6146678003332)},
//         {DECIMAL(513.780424677385),   DECIMAL(410.31951218219683)},
//         {DECIMAL(655.9153967653705),  DECIMAL(182.76347341186144)},
//         {DECIMAL(340.53686956204075), DECIMAL(598.4563901280716)},
//         {DECIMAL(392.5920815577219),  DECIMAL(548.783429938299)},
//         {DECIMAL(443.72986548528297), DECIMAL(494.76101854907455)},
//         {DECIMAL(470.3974489413311),  DECIMAL(464.18744060291704)},
//         {DECIMAL(482.12392335127174), DECIMAL(450.15862072244306)},
//     };
//     SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), principleAxisDimensions, attitude);

//     Vec3 actual = algo.Run(edge_pts);
//     Vec3 expected = {4211.974212862954, 3307.209203472482, -5616.06183322837};

//     // Vec3 trueRp = {-5875.0, 4366.0, 2577.0};
//     // Vec3 computedRp = TPC.transpose() * actual;
//     // const Mat3 Ap = principleAxes_.cwiseInverse().asDiagonal();
//     // const Mat3 Ac = TPC.transpose() * Ap * TPC;
//     // decimal expectedDist = trueRp.norm();
//     // decimal actualDist = computedRp.norm();
//     // ASSERT_DECIMAL_EQ_DEFAULT(expectedDist, actualDist);

//     ASSERT_VEC2_EQ(actual, expected, 2000);
// }

// Circular spheroid (a = b = c) tests — orientation left as identity for now

#define RADIUS_OF_EARTH (DECIMAL(6378137.0))

TEST(SpheroidDistanceDeterminationAlgorithmTest, TestCenteredEarthX1) {
    Quaternion orientation = SphericalToQuaternion(DegToRad(180), DegToRad(0), DegToRad(0));
    Vec3 axes(RADIUS_OF_EARTH, RADIUS_OF_EARTH, RADIUS_OF_EARTH);

    decimal x_E = RADIUS_OF_EARTH + 1000000;
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 1, imageWidth, imageHeight);
    PositionVector expected = {x_E, 0, 0};

    decimal alpha = DECIMAL_ASIN(RADIUS_OF_EARTH / x_E);
    decimal p = sqrt(x_E * x_E - RADIUS_OF_EARTH * RADIUS_OF_EARTH);

    Vec3 p1 = {DECIMAL(p * DECIMAL_COS(alpha)), DECIMAL(p * DECIMAL_SIN(alpha)), 0};
    Vec3 p2 = {DECIMAL(p * DECIMAL_COS(alpha)), DECIMAL(-p * DECIMAL_SIN(alpha)), 0};
    Vec3 p3 = {DECIMAL(p * DECIMAL_COS(alpha)), 0, DECIMAL(p * DECIMAL_SIN(alpha))};

    Points pts = {cam.CameraToPixelCoordinates(p1),
                  cam.CameraToPixelCoordinates(p2),
                  cam.CameraToPixelCoordinates(p3)};

    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), axes, orientation);
    PositionVector actual = algo.Run(pts);

    ASSERT_VEC2_EQ(actual, expected, DEFAULT_TOLERANCE);
}

TEST(SpheroidDistanceDeterminationAlgorithmTest, TestCenteredEarthX2) {
    Quaternion orientation = SphericalToQuaternion(DegToRad(180), DegToRad(0), DegToRad(0));
    Vec3 axes(RADIUS_OF_EARTH, RADIUS_OF_EARTH, RADIUS_OF_EARTH);

    decimal x_E = RADIUS_OF_EARTH + 10000000;
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);
    PositionVector expected = {x_E, 0, 0};

    decimal alpha = DECIMAL_ASIN(RADIUS_OF_EARTH / x_E);
    decimal p = sqrt(x_E * x_E - RADIUS_OF_EARTH * RADIUS_OF_EARTH);
    decimal centerMag = DECIMAL(p * DECIMAL_COS(alpha));
    decimal projectionRadiusMag = DECIMAL(p * DECIMAL_SIN(alpha));

    Vec3 p1 = {centerMag, projectionRadiusMag * -DECIMAL_COS(0.1), projectionRadiusMag * DECIMAL_SIN(0.1)};
    Vec3 p2 = {centerMag, projectionRadiusMag, 0};
    Vec3 p3 = {centerMag, projectionRadiusMag * -DECIMAL_COS(-0.1), projectionRadiusMag * DECIMAL_SIN(-0.1)};

    Points pts = {cam.CameraToPixelCoordinates(p1),
                  cam.CameraToPixelCoordinates(p2),
                  cam.CameraToPixelCoordinates(p3)};

    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), axes, orientation);
    PositionVector actual = algo.Run(pts);

    ASSERT_VEC2_EQ(actual, expected, DEFAULT_TOLERANCE);
}

TEST(SpheroidDistanceDeterminationAlgorithmTest, TestCenteredEarthY1) {
    Quaternion orientation = SphericalToQuaternion(DegToRad(270), DegToRad(0), DegToRad(0));
    Vec3 axes(RADIUS_OF_EARTH, RADIUS_OF_EARTH, RADIUS_OF_EARTH);

    decimal x_E = 7000000;
    int imageWidth = 6000;
    int imageHeight = 6000;
    Camera cam(0.085, 0.00002, imageWidth, imageHeight);
    PositionVector expected = {0, x_E, 0};

    Points pts = {
        {DECIMAL(1040.29858052800545920035801827907562),
            DECIMAL(2153.41717327522883351775817573070526)},
        {DECIMAL(905.12458718505968136014416813850403),
            DECIMAL(4843.01654323685215786099433898925781)},
        {DECIMAL(649.50418565826112171635031700134277),
            DECIMAL(5992.19055700358148897066712379455566)},
    };

    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), axes, orientation);
    PositionVector actual = algo.Run(pts);

    ASSERT_VEC2_EQ(actual, expected, DEFAULT_TOLERANCE);
}

TEST(SpheroidDistanceDeterminationAlgorithmTest, TestCenteredEarthY2) {
    EulerAngles local(DECIMAL_M_PI / 4, 0, 0);
    EulerAngles reference(3 * DECIMAL_M_PI / 4, 0, 0);

    LOSTCalibrationAlgorithm algorithm;
    Quaternion localToRef = algorithm.Run(std::make_pair(local, reference));

    Quaternion orientation = SphericalToQuaternion(DegToRad(180), DegToRad(0), DegToRad(0));
    Vec3 axes(RADIUS_OF_EARTH, RADIUS_OF_EARTH, RADIUS_OF_EARTH);

    decimal x_E = 80000000;
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.0001, 0.00002, imageWidth, imageHeight);
    PositionVector expected = {0, x_E, 0};

    Points pts = {
        {DECIMAL(15.92071472869832859942107461392879),
            DECIMAL(551.36076804192668987525394186377525)},
        {DECIMAL(383.64131687430347028566757217049599),
            DECIMAL(520.96642804561145112529629841446877)},
        {DECIMAL(425.31380021720195827583665959537029),
            DECIMAL(516.80315691018540746881626546382904)},
    };

    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), axes, localToRef, orientation);
    PositionVector actual = algo.Run(pts);

    ASSERT_VEC2_EQ(actual, expected, DEFAULT_TOLERANCE);
}
