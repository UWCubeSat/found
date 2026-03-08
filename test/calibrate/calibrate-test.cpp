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
    EulerAngles local(9 * DECIMAL_M_PI / 5, 3 * DECIMAL_M_PI / 7, 7 * DECIMAL_M_PI / 9);
    EulerAngles reference(11 * DECIMAL_M_PI / 6, -1 * DECIMAL_M_PI / 4, 0);

    LOSTCalibrationAlgorithm algorithm;
    Quaternion result = algorithm.Run(std::make_pair(local, reference));

    // Quaternion actualReference = result * SphericalToQuaternion(local);

    // // See if multiplying the reference to the relative orientation
    // // gives the local orientation back.
    // ASSERT_QUAT_EQ_DEFAULT(SphericalToQuaternion(reference), actualReference);

    Quaternion diffReference = SphericalToQuaternion(4.4, 2.9, 0.7);
    Quaternion diffLocal = result * diffReference;

    // See if the calibration holds for different axes
    ASSERT_QUAT_EQ_DEFAULT((SphericalToQuaternion(local) * SphericalToQuaternion(reference).conjugate()).conjugate(),
                           (diffLocal * diffReference.conjugate()));
}

}  // namespace found
