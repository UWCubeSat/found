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

TEST(AttitudeUtilsTest, SphericalToQuaternionEarthToCameraTest) {
    Mat3 earthCenteredInertial = (Mat3() << 1, 0, 0,
                                            0, 1, 0,
                                            0, 0, 1).finished();

    Mat3 cameraAlongVernalEquinox = (Mat3() <<  0, 0, -1,
                                                1, 0, 0,
                                                0, -1, 0).finished();
    
    Quaternion cameraAlongVernalEquinoxQuat = SphericalToQuaternion(DegToRad(180), DegToRad(0), DegToRad(270));

    Mat3 earthToCamera = cameraAlongVernalEquinoxQuat.toRotationMatrix() * 
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
    
    Quaternion cameraAlongVernalEquinoxQuat = SphericalToQuaternion(DegToRad(180), DegToRad(0), DegToRad(270));

    Mat3 cameraToEarth = cameraAlongVernalEquinoxQuat.conjugate().toRotationMatrix() * 
        cameraAlongVernalEquinox;

    ASSERT_MAT3_EQ_DEFAULT(earthCenteredInertial, cameraToEarth);
}

}