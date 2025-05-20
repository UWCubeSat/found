#include <gtest/gtest.h>
#include <gmock/gmock.h>

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

    EulerAngles expected(7 * DECIMAL_M_PI / 4, 0, 0);

    LOSTCalibrationAlgorithm algorithm;
    EulerAngles actual = algorithm.Run(std::make_pair(local, reference)).Canonicalize().ToSpherical();

    ASSERT_EA_EQ_DEFAULT(expected, actual);
}

TEST(CalibrationTest, TestCalibrateRelativeSimple2) {
    EulerAngles local(DECIMAL_M_PI / 3, 0, 0);
    EulerAngles reference(DECIMAL_M_PI / 3, -DECIMAL_M_PI / 6, 0);

    EulerAngles expected(0, DECIMAL_M_PI / 6, 0);

    LOSTCalibrationAlgorithm algorithm;
    EulerAngles actual = algorithm.Run(std::make_pair(local, reference)).Canonicalize().ToSpherical();

    ASSERT_EA_EQ_DEFAULT(expected, actual);
}

}  // namespace found
