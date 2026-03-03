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
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(5.0), Distance(a, b));
}

TEST(AttitudeUtilsTest, TestQuaternionToSphericalIdentity) {
    EulerAngles expected(DegToRad(0), DegToRad(90), DegToRad(0));
    EulerAngles angles = QuaternionToSpherical(Quaternion::Identity());

    ASSERT_DECIMAL_EQ_DEFAULT(expected.x(), angles.x());
    ASSERT_DECIMAL_EQ_DEFAULT(expected.y(), angles.y());
    ASSERT_DECIMAL_EQ_DEFAULT(expected.z(), angles.z());
}

TEST(AttitudeUtilsTest, TestQuaternionNorthPole) {
    EulerAngles expected(DegToRad(0), DegToRad(90), DegToRad(20));
    EulerAngles angles = QuaternionToSpherical(Quaternion::Identity() *
        Quaternion(Eigen::AngleAxis<decimal>(DegToRad(20), Vec3::UnitZ())));

    ASSERT_DECIMAL_EQ_DEFAULT(expected.x(), angles.x());
    ASSERT_DECIMAL_EQ_DEFAULT(expected.y(), angles.y());
    ASSERT_DECIMAL_EQ_DEFAULT(expected.z(), angles.z());
}

TEST(AttitudeUtilsTest, TestQuaternionSouthPole) {
    EulerAngles expected(DegToRad(0), DegToRad(-90), DegToRad(170));
    EulerAngles angles = QuaternionToSpherical(
        Quaternion(Eigen::AngleAxis<decimal>(DegToRad(-180), Vec3::UnitY())) *
        Quaternion(Eigen::AngleAxis<decimal>(DegToRad(190), Vec3::UnitZ())));

    ASSERT_DECIMAL_EQ_DEFAULT(expected.x(), angles.x());
    ASSERT_DECIMAL_EQ_DEFAULT(expected.y(), angles.y());
    ASSERT_DECIMAL_EQ_DEFAULT(expected.z(), angles.z());
}

TEST(AttitudeUtilsTest, TestQuaternionNorthPoleNegativeRollNormalization) {
    Quaternion quat(Eigen::AngleAxis<decimal>(DegToRad(-60), Vec3::UnitZ()));
    EulerAngles angles = QuaternionToSpherical(quat);

    ASSERT_DECIMAL_EQ_DEFAULT(DegToRad(0),   angles.x());
    ASSERT_DECIMAL_EQ_DEFAULT(DegToRad(90),  angles.y());
    ASSERT_DECIMAL_EQ_DEFAULT(DegToRad(300), angles.z());
}

TEST(AttitudeUtilsTest, TestQuaternionSouthPoleNegativeRollNormalization) {
    // roll=20° causes -2*atan2(x,y) < 0, exercising the south-pole normalization branch.
    EulerAngles expected(DegToRad(0), DegToRad(-90), DegToRad(20));
    Quaternion quat = SphericalToQuaternion(expected);
    EulerAngles angles = QuaternionToSpherical(quat);

    ASSERT_DECIMAL_EQ_DEFAULT(expected.x(), angles.x());
    ASSERT_DECIMAL_EQ_DEFAULT(expected.y(), angles.y());
    ASSERT_DECIMAL_EQ_DEFAULT(expected.z(), angles.z());
}

TEST(AttitudeUtilsTest, TestQuaternionNegativeRaNormalization) {
    // ra=350° causes atan2(yz+wx, xz-wy) < 0, exercising the general-case ra normalization branch.
    decimal ra = DegToRad(350);
    decimal dec = DegToRad(-30);
    decimal roll = DegToRad(45);

    Quaternion quat = SphericalToQuaternion(ra, dec, roll);
    EulerAngles angles = QuaternionToSpherical(quat);

    ASSERT_DECIMAL_EQ_DEFAULT(ra, angles.x());
    ASSERT_DECIMAL_EQ_DEFAULT(dec, angles.y());
    ASSERT_DECIMAL_EQ_DEFAULT(roll, angles.z());
}

TEST(AttitudeUtilsTest, TestConversionAreInverse) {
    decimal ra = DegToRad(123.4);
    decimal dec = DegToRad(-56.7);
    decimal roll = DegToRad(89.0);

    Quaternion quat = SphericalToQuaternion(ra, dec, roll);
    EulerAngles angles = QuaternionToSpherical(quat);

    ASSERT_DECIMAL_EQ_DEFAULT(ra, angles.x());
    ASSERT_DECIMAL_EQ_DEFAULT(dec, angles.y());
    ASSERT_DECIMAL_EQ_DEFAULT(roll, angles.z());
}

TEST(AttitudeUtilsTest, SphericalToQuaternionEarthToCameraTest) {
    Mat3 earthCenteredInertial = (Mat3() << 1, 0, 0,
                                            0, 1, 0,
                                            0, 0, 1).finished();

    Mat3 cameraAlongVernalEquinox = (Mat3() <<  0, 0, -1,
                                                1, 0, 0,
                                                0, -1, 0).finished();

    Quaternion cameraAlongVernalEquinoxQuat = SphericalToQuaternion(DegToRad(180), DegToRad(0), DegToRad(90));

    Mat3 earthToCamera = cameraAlongVernalEquinoxQuat.conjugate().toRotationMatrix() *
        earthCenteredInertial;

    ASSERT_MAT3_EQ_DEFAULT(cameraAlongVernalEquinox, earthToCamera);
}

TEST(AttitudeUtilsTest, SphericalToQuaternionCameraToEarthTest) {
    Mat3 earthCenteredInertial = (Mat3() << 1, 0, 0,
                                            0, 1, 0,
                                            0, 0, 1).finished();

    Mat3 cameraAlongVernalEquinox = (Mat3() <<  0, 0, -1,
                                                1, 0, 0,
                                                0, -1, 0).finished();

    Quaternion cameraAlongVernalEquinoxQuat = SphericalToQuaternion(DegToRad(180), DegToRad(0), DegToRad(90));

    Mat3 cameraToEarth = cameraAlongVernalEquinoxQuat.toRotationMatrix() *
        cameraAlongVernalEquinox;

    ASSERT_MAT3_EQ_DEFAULT(earthCenteredInertial, cameraToEarth);
}

}  // namespace found
