#include <gtest/gtest.h>

#include "test/common/common.hpp"

#include "src/common/decimal.hpp"

#include "src/distance/output.hpp"

namespace found {

TEST(DistanceOutputTest, TestGetEarthCoordinatesNoRotationSimpleVector) {
    Vec3 celestialVector{1, 0, 0};
    decimal gmst = 0;
    ECEFCoordinates actual = GetEarthCoordinates(celestialVector, gmst);
    ECEFCoordinates expected = {DECIMAL_M_A_E, 0, 0};

    ASSERT_DECIMAL_EQ_DEFAULT(expected.x, actual.x);
    ASSERT_DECIMAL_EQ_DEFAULT(expected.y, actual.y);
    ASSERT_DECIMAL_EQ_DEFAULT(expected.z, actual.z);
}

TEST(DistanceOutputTest, TestGetEarthCoordinates90RotationSimpleVector) {
    Vec3 celestialVector{1, 0, 0};
    decimal gmst = 90;
    ECEFCoordinates actual = GetEarthCoordinates(celestialVector, gmst);
    ECEFCoordinates expected = {6375756.24, 0, -173668.35};

    ASSERT_DECIMAL_EQ_DEFAULT(expected.x, actual.x);
    ASSERT_DECIMAL_EQ_DEFAULT(expected.y, actual.y);
    ASSERT_DECIMAL_EQ_DEFAULT(expected.z, actual.z);
}

TEST(DistanceOutputTest, TestGetEarthCoordinatesGeneralRotationSimpleVector) {
    Vec3 celestialVector{1, 0, 0};
    decimal gmst = 291;
    ECEFCoordinates actual = GetEarthCoordinates(celestialVector, gmst);
    ECEFCoordinates expected = {6353261.31, 0, 560876.32};

    ASSERT_DECIMAL_EQ_DEFAULT(expected.x, actual.x);
    ASSERT_DECIMAL_EQ_DEFAULT(expected.y, actual.y);
    ASSERT_DECIMAL_EQ_DEFAULT(expected.z, actual.z);
}

TEST(DistanceOutputTest, TestGetEarthCoordinatesGeneral1) {
    Vec3 celestialVector{1, 2, 3};
    decimal gmst = 54;
    ECEFCoordinates actual = GetEarthCoordinates(celestialVector, gmst);
    ECEFCoordinates expected = {6377270.16, 103552.65, 18208.34};

    ASSERT_DECIMAL_EQ_DEFAULT(expected.x, actual.x);
    ASSERT_DECIMAL_EQ_DEFAULT(expected.y, actual.y);
    ASSERT_DECIMAL_EQ_DEFAULT(expected.z, actual.z);
}

TEST(DistanceOutputTest, TestGetEarthCoordinatesGeneral2) {
    Vec3 celestialVector{4.4, -3.9, 12.2};
    decimal gmst = 528;
    ECEFCoordinates actual = GetEarthCoordinates(celestialVector, gmst);
    ECEFCoordinates expected = {6370263.99, 124729.28, 290246.60};

    ASSERT_DECIMAL_EQ_DEFAULT(expected.x, actual.x);
    ASSERT_DECIMAL_EQ_DEFAULT(expected.y, actual.y);
    ASSERT_DECIMAL_EQ_DEFAULT(expected.z, actual.z);
}

}  // namespace found
