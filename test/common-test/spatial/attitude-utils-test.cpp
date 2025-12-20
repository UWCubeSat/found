#include <gtest/gtest.h>

#include "test/common/common.hpp"

#include "src/common/spatial/attitude-utils.hpp"
#include "src/common/decimal.hpp"

namespace found {

// Test EarthRadiusAtLatitude function
TEST(AttitudeUtilsTest, TestEarthRadiusAtLatitudeEquator) {
    decimal latitude = 0;  // Equator
    decimal actual = EarthRadiusAtLatitude(latitude);
    
    // At equator, geocentric radius should equal semi-major axis
    decimal expected = DECIMAL_M_A_E;
    
    ASSERT_DECIMAL_EQ_DEFAULT(expected, actual);
}

TEST(AttitudeUtilsTest, TestEarthRadiusAtLatitudeNorthPole) {
    decimal latitude = DECIMAL_M_PI / 2;  // North Pole
    decimal actual = EarthRadiusAtLatitude(latitude);
    
    // At North Pole, geocentric radius should equal semi-minor axis
    decimal expected = DECIMAL_M_B_E;
    
    ASSERT_DECIMAL_EQ_DEFAULT(expected, actual);
}

TEST(AttitudeUtilsTest, TestEarthRadiusAtLatitude45Degrees) {
    decimal latitude = DegToRad(45.0);  // 45 degrees
    decimal actual = EarthRadiusAtLatitude(latitude);
    decimal expected = 6367209;
    
    ASSERT_DECIMAL_EQ_DEFAULT(expected, actual);
}


// Test LLAToECEF function
TEST(AttitudeUtilsTest, TestLLAToECEFEquatorPrimeMeridian) {
    decimal latitude = 0;  // Equator
    decimal longitude = 0;
    decimal altitude = 0;  // On surface
    
    ECEFCoordinates actual = LLAToECEF(latitude, longitude, altitude);
    
    decimal expected_x = DECIMAL_M_A_E;
    decimal expected_y = DECIMAL(0.0);
    decimal expected_z = DECIMAL(0.0);
    
    ASSERT_DECIMAL_EQ_DEFAULT(expected_x, actual.x);
    ASSERT_DECIMAL_EQ_DEFAULT(expected_y, actual.y);
    ASSERT_DECIMAL_EQ_DEFAULT(expected_z, actual.z);
}

TEST(AttitudeUtilsTest, TestLLAToECEFNorthPole) {
    decimal latitude = DECIMAL_M_PI / 2;  // North Pole
    decimal longitude = 0;
    decimal altitude = 0;  // On surface
    
    ECEFCoordinates actual = LLAToECEF(latitude, longitude, altitude);
    
    decimal expected_x = DECIMAL(0.0);
    decimal expected_y = DECIMAL(0.0);
    decimal expected_z = DECIMAL_M_B_E;
    
    ASSERT_DECIMAL_EQ_DEFAULT(expected_x, actual.x);
    ASSERT_DECIMAL_EQ_DEFAULT(expected_y, actual.y);
    ASSERT_DECIMAL_EQ_DEFAULT(expected_z, actual.z);
}

TEST(AttitudeUtilsTest, TestLLAToECEFWithAltitude) {
    decimal latitude = DegToRad(45.0);
    decimal longitude = DegToRad(90.0);  // 90°E
    decimal altitude = 1000.0;  // 1 km altitude
    
    ECEFCoordinates actual = LLAToECEF(latitude, longitude, altitude);
    
    decimal expected_x = DECIMAL(0.0);
    decimal expected_y = 4518297.99;
    decimal expected_z = 4488055.52;
    
    ASSERT_DECIMAL_EQ_DEFAULT(expected_x, actual.x);
    ASSERT_DECIMAL_EQ_DEFAULT(expected_y, actual.y);
    ASSERT_DECIMAL_EQ_DEFAULT(expected_z, actual.z);
}

TEST(AttitudeUtilsTest, TestLLAToECEF90DegreesEast) {
    decimal latitude = 0;  // Equator
    decimal longitude = DegToRad(90.0);  // 90°E
    decimal altitude = 0;  // On surface
    
    ECEFCoordinates actual = LLAToECEF(latitude, longitude, altitude);
    
    decimal expected_x = DECIMAL(0.0);
    decimal expected_y = DECIMAL_M_A_E;
    decimal expected_z = DECIMAL(0.0);
    
    ASSERT_DECIMAL_EQ_DEFAULT(expected_x, actual.x);
    ASSERT_DECIMAL_EQ_DEFAULT(expected_y, actual.y);
    ASSERT_DECIMAL_EQ_DEFAULT(expected_z, actual.z);
}

}  // namespace found
