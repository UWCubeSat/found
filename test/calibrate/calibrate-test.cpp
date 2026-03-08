#include <gtest/gtest.h>

#include <utility>

#include "test/common/common.hpp"

#include "src/common/spatial/attitude-utils.hpp"
#include "src/common/decimal.hpp"

#include "src/calibrate/calibrate.hpp"

namespace found {

TEST(CalibrationTest, TestCalibrateAbsolute) {
    EulerAngles local(0, 0, 0);
    EulerAngles reference(DECIMAL_M_PI / 4, DECIMAL_M_PI / 6, DECIMAL_M_PI / 3);

    LOSTCalibrationAlgorithm algorithm;
    Quaternion actual = algorithm.Run(std::make_pair(local, reference));

    ASSERT_QUAT_EQ_DEFAULT(SphericalToQuaternion(reference), actual);
}

TEST(CalibrationTest, TestCalibrateRelativeSimple1) {
    EulerAngles local(DECIMAL_M_PI / 4, 0, 0);
    EulerAngles reference(DECIMAL_M_PI / 2, 0, 0);

    // We rotate PI/4
    EulerAngles expected(DECIMAL_M_PI / 4, 0, 0);

    LOSTCalibrationAlgorithm algorithm;
    Quaternion actual = algorithm.Run(std::make_pair(local, reference));

    ASSERT_QUAT_EQ_DEFAULT(SphericalToQuaternion(expected), actual);
}

TEST(CalibrationTest, TestCalibrateRelativeSimple2) {
    EulerAngles local(DECIMAL_M_PI / 3, 0, 0);
    EulerAngles reference(DECIMAL_M_PI / 3, - DECIMAL_M_PI / 4, 0);

    // -pi/4 rotation around transofrmed y axis
    Quaternion zRotation(AngleAxis(DECIMAL_M_PI / 3, Vec3(0, 0, 1)));
    Quaternion expected(AngleAxis(- DECIMAL_M_PI / 4,
                                  zRotation * Vec3(0, 1, 0)));

    LOSTCalibrationAlgorithm algorithm;
    Quaternion actual = algorithm.Run(std::make_pair(local, reference));

    ASSERT_QUAT_EQ_DEFAULT(expected, actual);
}

TEST(CalibrationTest, TestCalibrateGeneral) {
    EulerAngles local(     DegToRad(120),    DegToRad(20),     DegToRad(60));
    EulerAngles reference( DegToRad(330),    DegToRad(50),     DegToRad(120));
    // same offsets different angles
    EulerAngles local2(    DegToRad(120),    DegToRad(20),     DegToRad(60));
    EulerAngles reference2(DegToRad(330),    DegToRad(50),     DegToRad(120));

    LOSTCalibrationAlgorithm algorithm;
    Quaternion result = algorithm.Run(std::make_pair(local, reference));

    Quaternion actualReference = SphericalToQuaternion(local2) * result;

    // See if the calibration holds for different axes
    ASSERT_QUAT_EQ_DEFAULT(SphericalToQuaternion(reference2),
                           actualReference);
}

}  // namespace found
