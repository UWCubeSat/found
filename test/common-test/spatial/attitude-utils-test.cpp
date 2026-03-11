#include <gtest/gtest.h>

#include "test/common/common.hpp"

#include "src/common/spatial/attitude-utils.hpp"

namespace found {

TEST(AttitudeUtilsTest, Vec2MidpointTest) {
    Vec2 a(1.0, 2.0);
    Vec2 b(3.0, 4.0);
    Vec2 expected(2.0, 3.0);
    ASSERT_VEC2_EQ_DEFAULT(expected, Midpoint(a, b));
}

TEST(AttitudeUtilsTest, Vec3MidpointThreeArgTest) {
    Vec3 a(0.0, 0.0, 0.0);
    Vec3 b(3.0, 3.0, 3.0);
    Vec3 c(6.0, 6.0, 6.0);
    Vec3 expected(3.0, 3.0, 3.0);
    ASSERT_VEC3_EQ_DEFAULT(expected, Midpoint(a, b, c));
}

TEST(AttitudeUtilsTest, Vec2DistanceTest) {
    Vec2 a(0.0, 0.0);
    Vec2 b(3.0, 4.0);
    ASSERT_ANGLE_EQ_DEFAULT(DECIMAL(5.0), Distance(a, b));
}

// SphericalToQuaternionTests

TEST(AttitudeUtilsTest, TestSphericalToQuaternionRa) {
    decimal ra = DegToRad(90);
    decimal dec = DegToRad(0);
    decimal roll = DegToRad(0);

    Quaternion quat = SphericalToQuaternion(ra, dec, roll).conjugate();
    // the camera boresight is alway {1.0, 0.0, 0.0} in it's own coordinates since that is how it's defined
    Vec3 cameraBoresightCameraCoordinates = {1.0, 0.0, 0.0};
    // gives camera->equatorial coordinates
    // camera is pointing in y-direction after 90 degree rotation around z-axis
    Vec3 cameraBoresightWorldCoordinates = quat.conjugate() * cameraBoresightCameraCoordinates;

    Vec3 expected = {DECIMAL_COS(dec) * DECIMAL_COS(ra),
                     DECIMAL_COS(dec) * DECIMAL_SIN(ra),
                     DECIMAL_SIN(dec)};

    ASSERT_VEC3_EQ_DEFAULT(expected, cameraBoresightWorldCoordinates);
}

TEST(AttitudeUtilsTest, TestSphericalToQuaternionDec) {
    decimal ra = DegToRad(0);
    decimal dec = DegToRad(85);
    decimal roll = DegToRad(0);

    Quaternion quat = SphericalToQuaternion(ra, dec, roll).conjugate();
    Vec3 cameraBoresightCameraCoordinates = {1.0, 0.0, 0.0};

    // gives camera->equatorial coordinates
    Vec3 cameraBoresightWorldCoordinates = quat.conjugate() * cameraBoresightCameraCoordinates;

    Vec3 expected = {DECIMAL_COS(dec) * DECIMAL_COS(ra),
                     DECIMAL_COS(dec) * DECIMAL_SIN(ra),
                     DECIMAL_SIN(dec)};

    ASSERT_VEC3_EQ_DEFAULT(expected, cameraBoresightWorldCoordinates);
}

TEST(AttitudeUtilsTest, TestSphericalToQuaternionRaDec) {
    decimal ra = DegToRad(72);
    decimal dec = DegToRad(-83);
    decimal roll = DegToRad(0);

    Quaternion quat = SphericalToQuaternion(ra, dec, roll).conjugate();
    Vec3 cameraBoresightCameraCoordinates = {1.0, 0.0, 0.0};

    // gives camera->equatorial coordinates
    Vec3 cameraBoresightWorldCoordinates = quat.conjugate() * cameraBoresightCameraCoordinates;

    Vec3 expected = {DECIMAL_COS(dec) * DECIMAL_COS(ra),
                     DECIMAL_COS(dec) * DECIMAL_SIN(ra),
                     DECIMAL_SIN(dec)};

    ASSERT_VEC3_EQ_DEFAULT(expected, cameraBoresightWorldCoordinates);
}

TEST(AttitudeUtilsTest, TestSphericalToQuaternionRoll) {
    decimal ra = DegToRad(80);
    decimal dec = DegToRad(85);
    decimal roll = DegToRad(40);

    Quaternion quat = SphericalToQuaternion(ra, dec, roll).conjugate();
    Vec3 cameraBoresightCameraCoordinates = {1.0, 0.0, 0.0};

    // gives camera->equatorial coordinates
    Vec3 cameraBoresightWorldCoordinates = quat.conjugate() * cameraBoresightCameraCoordinates;

    Vec3 boresightExpected = {DECIMAL_COS(dec) * DECIMAL_COS(ra),
                              DECIMAL_COS(dec) * DECIMAL_SIN(ra),
                              DECIMAL_SIN(dec)};


    // roll does not affect the boresight direction
    ASSERT_VEC3_EQ_DEFAULT(boresightExpected, cameraBoresightWorldCoordinates);

    Vec3 cameraZAxesCameraCoordinates = {0.0, 0.0, 1.0};
    Vec3 cameraZAxesWorldCoordinates = quat.conjugate() * cameraZAxesCameraCoordinates;
    Vec3 zAxesExpected = {-DECIMAL_COS(ra) * DECIMAL_SIN(dec) * DECIMAL_COS(roll) + DECIMAL_SIN(ra) * DECIMAL_SIN(roll),
                          -DECIMAL_SIN(ra) * DECIMAL_SIN(dec) * DECIMAL_COS(roll) - DECIMAL_COS(ra) * DECIMAL_SIN(roll),
                           DECIMAL_COS(dec) * DECIMAL_COS(roll)};

    ASSERT_VEC3_EQ_DEFAULT(zAxesExpected, cameraZAxesWorldCoordinates);

    // check that axes are perpendicular
    ASSERT_DECIMAL_EQ_DEFAULT(cameraBoresightWorldCoordinates.dot(cameraZAxesWorldCoordinates), DECIMAL(0.0));
}
}  // namespace found
