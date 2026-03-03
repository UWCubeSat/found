#include <gtest/gtest.h>

#include <utility>

#include "test/common/common.hpp"

#include "src/common/spatial/attitude-utils.hpp"
#include "src/common/decimal.hpp"

#include "src/calibrate/calibrate.hpp"

namespace found {

TEST(CalibrationTest, TestCalibrateAbsolute) {
    EulerAngles local = QuaternionToSpherical(Quaternion(0.36, 0.48, 0.64, 0.48));
    EulerAngles reference(0, 0, 0);

    LOSTCalibrationAlgorithm algorithm;
    Quaternion result = algorithm.Run(std::make_pair(local, reference));

    // Round-trip: result * S2Q(reference) should recover S2Q(local)
    Quaternion roundTrip = result * SphericalToQuaternion(reference);
    Quaternion expectedQ = SphericalToQuaternion(local);
    if (roundTrip.w() < 0) roundTrip = Quaternion(-roundTrip.w(), -roundTrip.x(), -roundTrip.y(), -roundTrip.z());
    if (expectedQ.w() < 0) expectedQ = Quaternion(-expectedQ.w(), -expectedQ.x(), -expectedQ.y(), -expectedQ.z());
    ASSERT_QUAT_EQ_DEFAULT(expectedQ, roundTrip);
}

TEST(CalibrationTest, TestCalibrateRelativeSimple1) {
    EulerAngles local(DECIMAL_M_PI / 4, 0, 0);
    EulerAngles reference(DECIMAL_M_PI / 2, 0, 0);

    LOSTCalibrationAlgorithm algorithm;
    Quaternion result = algorithm.Run(std::make_pair(local, reference));

    // Round-trip: result * S2Q(reference) should recover S2Q(local)
    Quaternion roundTrip = result * SphericalToQuaternion(reference);
    Quaternion expectedQ = SphericalToQuaternion(local);
    if (roundTrip.w() < 0) roundTrip = Quaternion(-roundTrip.w(), -roundTrip.x(), -roundTrip.y(), -roundTrip.z());
    if (expectedQ.w() < 0) expectedQ = Quaternion(-expectedQ.w(), -expectedQ.x(), -expectedQ.y(), -expectedQ.z());
    ASSERT_QUAT_EQ_DEFAULT(expectedQ, roundTrip);
}

TEST(CalibrationTest, TestCalibrateRelativeSimple2) {
    EulerAngles local(DECIMAL_M_PI / 3, 0, 0);
    EulerAngles reference(DECIMAL_M_PI / 3, -DECIMAL_M_PI / 6, 0);

    LOSTCalibrationAlgorithm algorithm;
    Quaternion result = algorithm.Run(std::make_pair(local, reference));

    // Round-trip: result * S2Q(reference) should recover S2Q(local)
    Quaternion roundTrip = result * SphericalToQuaternion(reference);
    Quaternion expectedQ = SphericalToQuaternion(local);
    if (roundTrip.w() < 0) roundTrip = Quaternion(-roundTrip.w(), -roundTrip.x(), -roundTrip.y(), -roundTrip.z());
    if (expectedQ.w() < 0) expectedQ = Quaternion(-expectedQ.w(), -expectedQ.x(), -expectedQ.y(), -expectedQ.z());
    ASSERT_QUAT_EQ_DEFAULT(expectedQ, roundTrip);
}

TEST(CalibrationTest, TestCalibrateGeneral) {
    // dec values must be in [-pi/2, pi/2]
    EulerAngles local(9 * DECIMAL_M_PI / 5, DECIMAL_M_PI / 7, 7 * DECIMAL_M_PI / 9);
    EulerAngles reference(11 * DECIMAL_M_PI / 6, -DECIMAL_M_PI / 4, 0);

    LOSTCalibrationAlgorithm algorithm;
    Quaternion result = algorithm.Run(std::make_pair(local, reference));

    Quaternion expectedLocalQ = SphericalToQuaternion(local);
    Quaternion actualLocalQ = result * SphericalToQuaternion(reference);

    // See if multiplying the reference to the relative orientation
    // gives the local orientation back.
    ASSERT_QUAT_EQ_DEFAULT(expectedLocalQ, actualLocalQ);

    // Verify calibration holds for a different reference orientation
    Quaternion diffReference = SphericalToQuaternion(4.4, 1.2, 0.7);
    Quaternion diffLocal = DCMToQuaternion(QuaternionToDCM(result) * QuaternionToDCM(diffReference));

    // The calibration quaternion should be the same regardless of reference
    Quaternion lhs = expectedLocalQ * SphericalToQuaternion(reference).conjugate();
    Quaternion rhs = diffLocal * diffReference.conjugate();
    if (lhs.w() < 0) lhs = Quaternion(-lhs.w(), -lhs.x(), -lhs.y(), -lhs.z());
    if (rhs.w() < 0) rhs = Quaternion(-rhs.w(), -rhs.x(), -rhs.y(), -rhs.z());
    ASSERT_QUAT_EQ_DEFAULT(lhs, rhs);
}

}  // namespace found
