#include <gtest/gtest.h>

#include "test/common/common.hpp"

#include "src/common/decimal.hpp"

#include "src/distance/output.hpp"

namespace found {

TEST(DistanceOutputTest, TestGetEarthCoordinatesNoRotationSimpleVector) {
    Vec3 equatorialVector{1, 0, 0};
    decimal gmst = 0;
    EarthSphericalVec3 actual = GetEarthCoordinates(equatorialVector, gmst);
    EulerAngles expected = {0, 0, 0};

    ASSERT_DECIMAL_EQ_DEFAULT(expected.x() > DECIMAL_M_PI ? 2 * DECIMAL_M_PI - expected.x() : expected.x(),
                              DegToRad(actual.longitude));
    ASSERT_DECIMAL_EQ_DEFAULT(expected.y(), DegToRad(actual.lattitude));
    ASSERT_DECIMAL_EQ_DEFAULT(equatorialVector.norm(), actual.radius);
    ASSERT_DECIMAL_EQ_DEFAULT(gmst, actual.GMST);
}

TEST(DistanceOutputTest, TestGetEarthCoordinates90RotationSimpleVector) {
    Vec3 equatorialVector{1, 0, 0};
    decimal gmst = 90;
    EarthSphericalVec3 actual = GetEarthCoordinates(equatorialVector, gmst);
    EulerAngles expected = {-DECIMAL_M_PI / 2, 0, 0};

    ASSERT_DECIMAL_EQ_DEFAULT(expected.x() > DECIMAL_M_PI ? 2 * DECIMAL_M_PI - expected.x() : expected.x(),
                              DegToRad(actual.longitude));
    ASSERT_DECIMAL_EQ_DEFAULT(expected.y(), DegToRad(actual.lattitude));
    ASSERT_DECIMAL_EQ_DEFAULT(equatorialVector.norm(), actual.radius);
    ASSERT_DECIMAL_EQ_DEFAULT(gmst, actual.GMST);
}

TEST(DistanceOutputTest, TestGetEarthCoordinatesGeneralRotationSimpleVector) {
    Vec3 equatorialVector{1, 0, 0};
    decimal gmst = 291;
    EarthSphericalVec3 actual = GetEarthCoordinates(equatorialVector, gmst);
    EulerAngles expected = {DegToRad(gmst), 0, 0};

    ASSERT_DECIMAL_EQ_DEFAULT((expected.x() > DECIMAL_M_PI ? 2 * DECIMAL_M_PI - expected.x() : expected.x()),
                              DegToRad(actual.longitude));
    ASSERT_DECIMAL_EQ_DEFAULT(expected.y(), DegToRad(actual.lattitude));
    ASSERT_DECIMAL_EQ_DEFAULT(equatorialVector.norm(), actual.radius);
    ASSERT_DECIMAL_EQ_DEFAULT(gmst, actual.GMST);
}

TEST(DistanceOutputTest, TestGetEarthCoordinatesGeneral1) {
    Vec3 equatorialVector{1, 2, 3};
    decimal gmst = 54;
    EarthSphericalVec3 actual = GetEarthCoordinates(equatorialVector, gmst);
    EulerAngles expected = {-DegToRad(gmst) + DECIMAL_ATAN2(equatorialVector.y(), equatorialVector.x()),
                          DECIMAL_ASIN(equatorialVector.z()/equatorialVector.norm()),
                          0};

    ASSERT_DECIMAL_EQ_DEFAULT((expected.x() > DECIMAL_M_PI ? 2 * DECIMAL_M_PI - expected.x() : expected.x()),
                              DegToRad(actual.longitude));
    ASSERT_DECIMAL_EQ_DEFAULT(expected.y(), DegToRad(actual.lattitude));
    ASSERT_DECIMAL_EQ_DEFAULT(equatorialVector.norm(), actual.radius);
    ASSERT_DECIMAL_EQ_DEFAULT(gmst, actual.GMST);
}

TEST(DistanceOutputTest, TestGetEarthCoordinatesGeneral2) {
    Vec3 equatorialVector{4.4, -3.9, 12.2};
    decimal gmst = 528;
    EarthSphericalVec3 actual = GetEarthCoordinates(equatorialVector, gmst);
    EulerAngles expected = {-DegToRad(std::fmod(gmst, 360.0))
                                + DECIMAL_ATAN2(equatorialVector.y(), equatorialVector.x())
                                + 2 * DECIMAL_M_PI,  // I manually adjust for this
                          DECIMAL_ASIN(equatorialVector.z()/equatorialVector.norm()),
                          0};

    ASSERT_DECIMAL_EQ_DEFAULT((expected.x() > DECIMAL_M_PI ? 2 * DECIMAL_M_PI - expected.x() : expected.x()),
                              DegToRad(actual.longitude));
    ASSERT_DECIMAL_EQ_DEFAULT(expected.y(), DegToRad(actual.lattitude));
    ASSERT_DECIMAL_EQ_DEFAULT(equatorialVector.norm(), actual.radius);
    ASSERT_DECIMAL_EQ_DEFAULT(gmst, actual.GMST);
}

}  // namespace found
