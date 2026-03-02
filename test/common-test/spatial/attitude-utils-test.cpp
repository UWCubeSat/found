#include <gtest/gtest.h>

#include "test/common/common.hpp"

#include "src/common/spatial/attitude-utils.hpp"

namespace found {

TEST(AttitudeUtilsTest, SphericalToQuaternionEarthToCameraTest) {
    Mat3 earthCenteredInertial = (Mat3() << 1, 0, 0,
                                            0, 1, 0,
                                            0, 0, 1).finished();

    Mat3 cameraAlongVernalEquinox = (Mat3() <<  0, 0, -1,
                                                1, 0, 0,
                                                0, -1, 0).finished();
    
    Quaternion cameraAlongVernalEquinoxQuat = SphericalToQuaternion(180, 0, 270);

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
    
    Quaternion cameraAlongVernalEquinoxQuat = SphericalToQuaternion(180, 0, 270);

    Mat3 cameraToEarth = cameraAlongVernalEquinoxQuat.conjugate().toRotationMatrix() * 
        cameraAlongVernalEquinox;

    ASSERT_MAT3_EQ_DEFAULT(earthCenteredInertial, cameraToEarth);
}

}