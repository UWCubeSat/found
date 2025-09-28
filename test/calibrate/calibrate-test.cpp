#include <gtest/gtest.h>

#include <utility>

#include "test/common/common.hpp"

#include "src/common/spatial/attitude-utils.hpp"
#include "src/common/decimal.hpp"

#include "src/calibrate/calibrate.hpp"

namespace found {

TEST(CalibrationTest, TestCalibrateAbsolute) {
    EulerAngles local = Quaternion(0.36, 0.48, 0.64, 0.48).ToSpherical();
    EulerAngles reference(0, 0, 0);

    LOSTCalibrationAlgorithm algorithm;
    EulerAngles actual = algorithm.Run(std::make_pair(local, reference)).Canonicalize().ToSpherical();

    ASSERT_EA_EQ_DEFAULT(local, actual);
}

TEST(CalibrationTest, TestCalibrateRelativeSimple1) {
    EulerAngles local(DECIMAL_M_PI / 4, 0, 0);
    EulerAngles reference(DECIMAL_M_PI / 2, 0, 0);

    // We rotate -PI/4, or 7PI/4
    EulerAngles expected(7 * DECIMAL_M_PI / 4, 0, 0);

    LOSTCalibrationAlgorithm algorithm;
    EulerAngles actual = algorithm.Run(std::make_pair(local, reference)).ToSpherical();

    ASSERT_EA_EQ_DEFAULT(expected, actual);
}

// Because in forward rotations, the calibration quaternion no longer makes sense (in backwards, it does),
// we can no longer have an intuitive expected calibration quaternion taken from EulerAngles, but instead
// we will verify that Quaternion::ToSpherical is the inverse of SphericalToQuaternion

TEST(CalibrationTest, TestSphericalToQuaternionInverse) {
    EulerAngles local(DECIMAL_M_PI / 6, DECIMAL_M_PI / 2, DECIMAL_M_PI / 3);
    EulerAngles reference(DECIMAL_M_PI / 9, DECIMAL_M_PI / 8, DECIMAL_M_PI / 7);

    // Define SphericalToQuaternion as f and Quaternion::ToSpherical to be f^-1

    LOSTCalibrationAlgorithm algorithm;

    // Verify that x = f^-1(f(x))
    Quaternion expected = algorithm.Run(std::make_pair(local, reference));
    Quaternion actual = SphericalToQuaternion(expected.ToSpherical());
    ASSERT_QUAT_EQ_DEFAULT(expected, actual);

    // Verify that x = f(f^-1(x)))
    EulerAngles expectedEA = expected.ToSpherical();
    EulerAngles actualEA = SphericalToQuaternion(expectedEA).ToSpherical();

    ASSERT_EA_EQ_DEFAULT(expectedEA, actualEA);
    // Because we know that f is valid via its many uses within the integration-test.cpp
    // and vectorize-test.cpp, we can thus verify that f^-1 is valid as well
}

TEST(CalibrationTest, TestCalibrateGeneral) {
    EulerAngles local(9 * DECIMAL_M_PI / 5, 6 * DECIMAL_M_PI / 7, 7 * DECIMAL_M_PI / 9);
    EulerAngles reference(11 * DECIMAL_M_PI / 6, 3 * DECIMAL_M_PI / 4, 0);

    LOSTCalibrationAlgorithm algorithm;
    Quaternion result = algorithm.Run(std::make_pair(local, reference));

    // First, check if we're running the algorithm right
    Quaternion expectedLocalQ = SphericalToQuaternion(local);
    Quaternion expected = expectedLocalQ * SphericalToQuaternion(reference).Conjugate();
    ASSERT_QUAT_EQ_DEFAULT(expected, result);

    // See if multiplying the reference to the relative orientation
    // gives the local orientation back.
    Quaternion actualLocalQ = result * SphericalToQuaternion(reference);
    ASSERT_QUAT_EQ_DEFAULT(expectedLocalQ, actualLocalQ);

    // Finally, see if the calibration holds when we move to different axes
    Quaternion diffReference = SphericalToQuaternion(4.4, 2.9, 0.7);
    Quaternion diffLocal = DCMToQuaternion(QuaternionToDCM(result) * QuaternionToDCM(diffReference));

    Quaternion expectedRel = expectedLocalQ * SphericalToQuaternion(reference).Conjugate();
    Quaternion actualRel = diffLocal * diffReference.Conjugate();

    ASSERT_QUAT_EQ_DEFAULT(expectedRel, actualRel);
}

}  // namespace found
