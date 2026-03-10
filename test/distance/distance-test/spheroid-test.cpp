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

TEST(SpheroidDistanceDeterminationAlgorithmTest, TestRandomEarth1) {
    Vec3 principleAxisDimensions(RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_C);
    int imageWidth = 700;
    int imageHeight = 600;
    Camera cam(DECIMAL(0.0494546), DECIMAL(0.0000514286), imageWidth, imageHeight);
    Quaternion attitude = SphericalToQuaternion(DegToRad(180), DegToRad(0), DegToRad(0));
    Points edge_pts = {
        {DECIMAL(31.772727272727273),  DECIMAL(241.6907247305747)},
        {DECIMAL(63.54545454545455),   DECIMAL(150.05418664653905)},
        {DECIMAL(95.31818181818181),   DECIMAL(101.09794885083511)},
        {DECIMAL(127.0909090909091),   DECIMAL(66.25909940751106)},
        {DECIMAL(158.86363636363637),  DECIMAL(39.803737694871366)},
        {DECIMAL(190.63636363636363),  DECIMAL(19.350957823370518)},
        {DECIMAL(222.4090909090909),   DECIMAL(3.655304617935572)},
        {DECIMAL(476.5909090909091),   DECIMAL(3.2296195568791637)},
        {DECIMAL(508.3636363636364),   DECIMAL(18.789251345646008)},
        {DECIMAL(540.1363636363636),   DECIMAL(39.076993619844835)},
        {DECIMAL(571.9090909090909),   DECIMAL(65.31585267107718)},
        {DECIMAL(603.6818181818182),   DECIMAL(99.83260403882633)},
        {DECIMAL(635.4545454545455),   DECIMAL(148.17171721454616)},
        {DECIMAL(667.2272727272727),   DECIMAL(236.50848757029866)},
    };
    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), principleAxisDimensions, attitude);

    Vec3 actual = algo.Run(edge_pts);
    Vec3 expected = {DECIMAL(20000.0), DECIMAL(0.0), DECIMAL(0.0)};

    ASSERT_VEC3_EQ(actual, expected, DEFAULT_TOLERANCE * DECIMAL(10.0));
}

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
