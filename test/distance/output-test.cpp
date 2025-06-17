#include <gtest/gtest.h>

#include "test/common/common.hpp"

#include "src/common/decimal.hpp"

#include "src/distance/output.hpp"

namespace found {

TEST(DistanceOutputTest, TestGetEarthCoordinatesNoRotationSimpleVector) {
    Vec3 celestialVector{1, 0, 0};
    decimal gmst = 0;
    EarthSphericalVec3 actual = GetEarthCoordinates(celestialVector, gmst);
    EulerAngles expected = {0, 0, 0};

    ASSERT_DECIMAL_EQ_DEFAULT(expected.ra > DECIMAL_M_PI ? 2 * DECIMAL_M_PI - expected.ra : expected.ra,
                              DegToRad(actual.longitude));
    ASSERT_DECIMAL_EQ_DEFAULT(expected.de, DegToRad(actual.lattitude));
    ASSERT_DECIMAL_EQ_DEFAULT(celestialVector.Magnitude(), actual.altitude);
    ASSERT_DECIMAL_EQ_DEFAULT(gmst, actual.GMST);
}

TEST(DistanceOutputTest, TestGetEarthCoordinates90RotationSimpleVector) {
    Vec3 celestialVector{1, 0, 0};
    decimal gmst = 90;
    EarthSphericalVec3 actual = GetEarthCoordinates(celestialVector, gmst);
    EulerAngles expected = {-DECIMAL_M_PI / 2, 0, 0};

    ASSERT_DECIMAL_EQ_DEFAULT(expected.ra > DECIMAL_M_PI ? 2 * DECIMAL_M_PI - expected.ra : expected.ra,
                              DegToRad(actual.longitude));
    ASSERT_DECIMAL_EQ_DEFAULT(expected.de, DegToRad(actual.lattitude));
    ASSERT_DECIMAL_EQ_DEFAULT(celestialVector.Magnitude(), actual.altitude);
    ASSERT_DECIMAL_EQ_DEFAULT(gmst, actual.GMST);
}

TEST(DistanceOutputTest, TestGetEarthCoordinatesGeneralRotationSimpleVector) {
    Vec3 celestialVector{1, 0, 0};
    decimal gmst = 291;
    EarthSphericalVec3 actual = GetEarthCoordinates(celestialVector, gmst);
    EulerAngles expected = {DegToRad(gmst), 0, 0};

    ASSERT_DECIMAL_EQ_DEFAULT((expected.ra > DECIMAL_M_PI ? 2 * DECIMAL_M_PI - expected.ra : expected.ra),
                              DegToRad(actual.longitude));
    ASSERT_DECIMAL_EQ_DEFAULT(expected.de, DegToRad(actual.lattitude));
    ASSERT_DECIMAL_EQ_DEFAULT(celestialVector.Magnitude(), actual.altitude);
    ASSERT_DECIMAL_EQ_DEFAULT(gmst, actual.GMST);
}

TEST(DistanceOutputTest, TestGetEarthCoordinatesGeneral1) {
    Vec3 celestialVector{1, 2, 3};
    decimal gmst = 54;
    EarthSphericalVec3 actual = GetEarthCoordinates(celestialVector, gmst);
    EulerAngles expected = {-DegToRad(gmst) + DECIMAL_ATAN2(celestialVector.y, celestialVector.x),
                          DECIMAL_ASIN(celestialVector.z/celestialVector.Magnitude()),
                          0};

    ASSERT_DECIMAL_EQ_DEFAULT((expected.ra > DECIMAL_M_PI ? 2 * DECIMAL_M_PI - expected.ra : expected.ra),
                              DegToRad(actual.longitude));
    ASSERT_DECIMAL_EQ_DEFAULT(expected.de, DegToRad(actual.lattitude));
    ASSERT_DECIMAL_EQ_DEFAULT(celestialVector.Magnitude(), actual.altitude);
    ASSERT_DECIMAL_EQ_DEFAULT(gmst, actual.GMST);
}

TEST(DistanceOutputTest, TestGetEarthCoordinatesGeneral2) {
    Vec3 celestialVector{4.4, -3.9, 12.2};
    decimal gmst = 528;
    EarthSphericalVec3 actual = GetEarthCoordinates(celestialVector, gmst);
    EulerAngles expected = {-DegToRad(std::fmod(gmst, 360.0))
                                + DECIMAL_ATAN2(celestialVector.y, celestialVector.x)
                                + 2 * DECIMAL_M_PI,  // I manually adjust for this
                          DECIMAL_ASIN(celestialVector.z/celestialVector.Magnitude()),
                          0};

    ASSERT_DECIMAL_EQ_DEFAULT((expected.ra > DECIMAL_M_PI ? 2 * DECIMAL_M_PI - expected.ra : expected.ra),
                              DegToRad(actual.longitude));
    ASSERT_DECIMAL_EQ_DEFAULT(expected.de, DegToRad(actual.lattitude));
    ASSERT_DECIMAL_EQ_DEFAULT(celestialVector.Magnitude(), actual.altitude);
    ASSERT_DECIMAL_EQ_DEFAULT(gmst, actual.GMST);
}

}  // namespace found
