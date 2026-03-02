// #include <gtest/gtest.h>

// #include <utility>

// #include "test/common/common.hpp"

// #include "src/common/spatial/attitude-utils.hpp"
// #include "src/common/decimal.hpp"

// #include "src/calibrate/calibrate.hpp"

// namespace found {

// TEST(CalibrationTest, TestCalibrateAbsolute) {
//     EulerAngles local = QuaternionToSpherical(Quaternion(0.36, 0.48, 0.64, 0.48));
//     EulerAngles reference(0, 0, 0);

//     LOSTCalibrationAlgorithm algorithm;
//     Quaternion result = algorithm.Run(std::make_pair(local, reference));
//     if (result.w() < 0) result = Quaternion(-result.w(), -result.x(), -result.y(), -result.z());
//     EulerAngles actual = QuaternionToSpherical(result);

//     ASSERT_EA_EQ_DEFAULT(local, actual);
// }

// TEST(CalibrationTest, TestCalibrateRelativeSimple1) {
//     EulerAngles local(DECIMAL_M_PI / 4, 0, 0);
//     EulerAngles reference(DECIMAL_M_PI / 2, 0, 0);

//     // We rotate -PI/4, or 7PI/4
//     EulerAngles expected(7 * DECIMAL_M_PI / 4, 0, 0);

//     LOSTCalibrationAlgorithm algorithm;
//     Quaternion result1 = algorithm.Run(std::make_pair(local, reference));
//     if (result1.w() < 0) result1 = Quaternion(-result1.w(), -result1.x(), -result1.y(), -result1.z());
//     EulerAngles actual = QuaternionToSpherical(result1);

//     ASSERT_EA_EQ_DEFAULT(expected, actual);
// }

// TEST(CalibrationTest, TestCalibrateRelativeSimple2) {
//     EulerAngles local(DECIMAL_M_PI / 3, 0, 0);
//     EulerAngles reference(DECIMAL_M_PI / 3, -DECIMAL_M_PI / 6, 0);

//     EulerAngles expected(0, DECIMAL_M_PI / 6, 0);

//     LOSTCalibrationAlgorithm algorithm;
//     Quaternion result2 = algorithm.Run(std::make_pair(local, reference));
//     if (result2.w() < 0) result2 = Quaternion(-result2.w(), -result2.x(), -result2.y(), -result2.z());
//     EulerAngles actual2 = QuaternionToSpherical(result2);

//     ASSERT_EA_EQ_DEFAULT(expected, actual2);
// }

// TEST(CalibrationTest, TestCalibrateGeneral) {
//     EulerAngles local(9 * DECIMAL_M_PI / 5, 6 * DECIMAL_M_PI / 7, 7 * DECIMAL_M_PI / 9);
//     EulerAngles reference(11 * DECIMAL_M_PI / 6, 3 * DECIMAL_M_PI / 4, 0);

//     LOSTCalibrationAlgorithm algorithm;
//     Quaternion result = algorithm.Run(std::make_pair(local, reference));

//     Quaternion expectedLocalQ = SphericalToQuaternion(local);
//     Quaternion actualLocalQ = result * SphericalToQuaternion(reference);

//     // See if multiplying the reference to the relative orientation
//     // gives the local orientation back.
//     ASSERT_QUAT_EQ_DEFAULT(expectedLocalQ, actualLocalQ);

//     Quaternion diffReference = SphericalToQuaternion(4.4, 2.9, 0.7);
//     Quaternion diffLocal = DCMToQuaternion(QuaternionToDCM(result) * QuaternionToDCM(diffReference));

//     // See if the calibration holds for different axes
//     ASSERT_QUAT_EQ_DEFAULT((expectedLocalQ * SphericalToQuaternion(reference).conjugate()),
//                            (diffLocal * diffReference.conjugate()));
// }

// }  // namespace found
