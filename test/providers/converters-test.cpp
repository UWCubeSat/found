#include <gtest/gtest.h>

#include <getopt.h>

#include <string>
#include <cstdio>
#include <fstream>
#include <memory>

#include "test/common/common.hpp"

#include "src/datafile/datafile.hpp"
#include "src/datafile/serialization.hpp"
#include "src/common/spatial/attitude-utils.hpp"
#include "src/providers/converters.hpp"
#include "src/common/decimal.hpp"
#include "src/common/time/time.hpp"

namespace found {

TEST(ConvertersTest, TestEAComma) {
    std::string str = "0,45,90";
    EulerAngles angles = strtoea(str);

    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), angles.ra);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL_M_PI / 4, angles.de);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL_M_PI / 2, angles.roll);
}

TEST(ConvertersTest, TestEASpace) {
    std::string str = "4.2 3.9 -9.5";
    EulerAngles angles = strtoea(str);

    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(DegToRad(4.2)), angles.ra);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(DegToRad(3.9)), angles.de);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(DegToRad(-9.5)), angles.roll);
}

TEST(ConvertersTest, TestEAIncomplete) {
    std::string str = "4.2 3.9";
    EulerAngles angles = strtoea(str);

    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(DegToRad(4.2)), angles.ra);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(DegToRad(3.9)), angles.de);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(DegToRad(0)), angles.roll);
}

TEST(ConvertersTest, TestBoolFalse) {
    ASSERT_FALSE(strtobool(""));
    ASSERT_FALSE(strtobool("0"));
    ASSERT_FALSE(strtobool("false"));
}

TEST(ConvertersTest, TestBoolTrue) {
    ASSERT_TRUE(strtobool("1"));
    ASSERT_TRUE(strtobool("true"));
    ASSERT_TRUE(strtobool("not_false"));
}

TEST(ConvertersTest, TestDecimal) {
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0.9876), strtodecimal("0.9876"));
}

TEST(ConvertersTest, TestUnsignedChar) {
    ASSERT_EQ(172, strtouc("172"));
}

TEST(ConvertersTest, TestImageNotExistentImage) {
    ASSERT_THROW(strtoimage("not_existent.png"), std::runtime_error);
}

TEST(ConvertersTest, TestImageNormalImage) {
    Image image = strtoimage("test/common/assets/example_image.jpg");

    ASSERT_EQ(1008, image.width);
    ASSERT_EQ(720, image.height);
    ASSERT_EQ(3, image.channels);
    ASSERT_NE(nullptr, image.image);

    stbi_image_free(image.image);  // Free the image memory
}

TEST(ConvertersTest, TestDataFileNonExistent) {
    ASSERT_THROW(strtodf("test/common/assets/non-existent.found"), std::runtime_error);
}

TEST(ConvertersTest, TestDataFileNormal) {
    DataFile expected{
        {{'F', 'O', 'U', 'N'}, 1U, 5},
        Quaternion(-0.26, 8.5, 0, 9.2),
        std::unique_ptr<LocationRecord[]>(new LocationRecord[5]{
            {45, {95.21, -62.15, 62.14}},
            {62, {623.25, -6182.9, -361.2}},
            {821, {623.26, 86.18, -105.21}},
            {926, {156.16, -296.29, 682.21}},
            {1062, {61.16, -168.21, -181.21}}
        })
    };
    std::ofstream file(temp_df);
    serializeDataFile(expected, file);
    file.flush();  // Write out all file contents
    DataFile actual = strtodf(temp_df);

    ASSERT_DF_EQ_DEFAULT(expected, actual);

    std::remove(temp_df);
}

TEST(ConvertersTest, TestLocationRecordsNonExistent) {
    ASSERT_THROW(strtolr("not_existent.txt"), std::runtime_error);
    ASSERT_THROW(strtolr(".txt"), std::runtime_error);
    ASSERT_THROW(strtolr("not_existent.found"), std::runtime_error);
}

TEST(ConvertersTest, TestLocationRecordsCSV) {
    ASSERT_THROW(strtolr("test/common/assets/position-data/pos-data-invalid.csv"), std::runtime_error);
}

TEST(ConvertersTest, TestLocationRecordsBadTextFormat) {
    ASSERT_THROW(strtolr("test/common/assets/position-data/pos-data-missing-num.txt"), std::runtime_error);
}

TEST(ConvertersTest, TestLocationRecordsNormal) {
    LocationRecords actual = strtolr(pos_data);

    ASSERT_EQ(static_cast<size_t>(3), actual.size());
    ASSERT_TRUE(LocationRecordEqual({1000000, {1.0, 2.0, 3.0}}, actual[0]));
    ASSERT_TRUE(LocationRecordEqual({2000000, {4.0, 5.0, 6.0}}, actual[1]));
    ASSERT_TRUE(LocationRecordEqual({3000000, {7.0, 8.0, 9.0}}, actual[2]));
}

TEST(ConvertersTest, TestLocationRecordsDataFile) {
    DataFile expected{
        {{'F', 'O', 'U', 'N'}, 1U, 3},
        Quaternion(1, 0, 0, 0),
        std::unique_ptr<LocationRecord[]>(new LocationRecord[3]{
            {45, {95.21, -62.15, 62.14}},
            {62, {623.25, -6182.9, -361.2}},
            {821, {623.26, 86.18, -105.21}}
        })
    };
    std::ofstream file(temp_df);
    serializeDataFile(expected, file);
    file.flush();  // Write out all file contents
    LocationRecords actual = strtolr(temp_df);

    ASSERT_EQ(static_cast<size_t>(3), actual.size());
    ASSERT_TRUE(LocationRecordEqual(expected.positions[0], actual[0]));
    ASSERT_TRUE(LocationRecordEqual(expected.positions[1], actual[1]));
    ASSERT_TRUE(LocationRecordEqual(expected.positions[2], actual[2]));

    std::remove(temp_df);
}

TEST(ConvertersTest, TestDateTimeRecent) {
    std::string str = "2024-02-29 14:30:45";  // Thu, Feb 29, 2024 (leap year)
    DateTime dt = strtodatetime(str);

    ASSERT_EQ(2024, dt.year);
    ASSERT_EQ(2, dt.month);
    ASSERT_EQ(29, dt.day);
    ASSERT_EQ(14, dt.hour);
    ASSERT_EQ(30, dt.minute);
    ASSERT_EQ(45, dt.second);
    ASSERT_EQ(0, dt.nanosecond);

    // Calculate expected epoch
    std::tm tm = {};
    tm.tm_year = 2024 - 1900;
    tm.tm_mon = 2 - 1;
    tm.tm_mday = 29;
    tm.tm_hour = 14;
    tm.tm_min = 30;
    tm.tm_sec = 45;
    std::time_t t = timegm(&tm);
    uint64_t expected_epochs = static_cast<uint64_t>(t) * NS_PER_SEC;
    ASSERT_EQ(expected_epochs, dt.epochs);
}

TEST(ConvertersTest, TestDateTimeCompletelyWrong) {
    std::string str = "not a date";
    ASSERT_THROW(strtodatetime(str), std::invalid_argument);
}


TEST(ConvertersTest, TestDateTimeInvalidDayInMonth) {
    std::string str = "2026-02-30 14:30:45";
    ASSERT_THROW(strtodatetime(str), std::invalid_argument);
}

TEST(ConvertersTest, TestDateTimeNoNanoseconds) {
    std::string str = "2030-09-12 13:20:12";
    DateTime dt = strtodatetime(str);

    ASSERT_EQ(2030, dt.year);
    ASSERT_EQ(9, dt.month);
    ASSERT_EQ(12, dt.day);
    ASSERT_EQ(13, dt.hour);
    ASSERT_EQ(20, dt.minute);
    ASSERT_EQ(12, dt.second);
    ASSERT_EQ(0, dt.nanosecond);

    // Calculate expected epoch
    std::tm tm = {};
    tm.tm_year = 2030 - 1900;
    tm.tm_mon = 9 - 1;
    tm.tm_mday = 12;
    tm.tm_hour = 13;
    tm.tm_min = 20;
    tm.tm_sec = 12;
    std::time_t t = timegm(&tm);
    uint64_t expected_epochs = static_cast<uint64_t>(t) * NS_PER_SEC;
    ASSERT_EQ(expected_epochs, dt.epochs);
}

TEST(ConvertersTest, TestDateTimeInvalidNanoseconds) {
    std::string str = "2024-12-22 14:30:45.abc";
    ASSERT_THROW(strtodatetime(str), std::invalid_argument);
}


TEST(ConvertersTest, TestDateTimeInvalidSecondTooHigh) {
    std::string str = "2024-12-22 14:30:60";  // Second 60 (parses but invalid)
    ASSERT_THROW(strtodatetime(str), std::invalid_argument);
}


TEST(ConvertersTest, TestDateTimeInvalidDayTooHigh) {
    std::string str = "2024-12-32 14:30:45";  // Day 32 for December
    ASSERT_THROW(strtodatetime(str), std::invalid_argument);
}

TEST(ConvertersTest, TestDateTimeInvalidNonLeapYearFeb29) {
    std::string str = "2025-02-29 14:30:45";  // Feb 29 in non-leap year 2025
    ASSERT_THROW(strtodatetime(str), std::invalid_argument);
}

TEST(ConvertersTest, TestDateTimeValidNanoseconds) {
    std::string str = "2024-12-22 14:30:45.123456789";
    DateTime dt = strtodatetime(str);

    ASSERT_EQ(2024, dt.year);
    ASSERT_EQ(12, dt.month);
    ASSERT_EQ(22, dt.day);
    ASSERT_EQ(14, dt.hour);
    ASSERT_EQ(30, dt.minute);
    ASSERT_EQ(45, dt.second);
    ASSERT_EQ(123456789, dt.nanosecond);

    // Calculate expected epoch
    std::tm tm = {};
    tm.tm_year = 2024 - 1900;
    tm.tm_mon = 12 - 1;
    tm.tm_mday = 22;
    tm.tm_hour = 14;
    tm.tm_min = 30;
    tm.tm_sec = 45;
    std::time_t t = timegm(&tm);
    uint64_t expected_epochs = static_cast<uint64_t>(t) * NS_PER_SEC + 123456789;
    ASSERT_EQ(expected_epochs, dt.epochs);
}

TEST(ConvertersTest, TestDateTimeValidNanosecondsShort) {
    std::string str = "2024-12-22 14:30:45.123";  // Short nanoseconds, should be padded
    DateTime dt = strtodatetime(str);

    ASSERT_EQ(2024, dt.year);
    ASSERT_EQ(12, dt.month);
    ASSERT_EQ(22, dt.day);
    ASSERT_EQ(14, dt.hour);
    ASSERT_EQ(30, dt.minute);
    ASSERT_EQ(45, dt.second);
    ASSERT_EQ(123000000, dt.nanosecond);  // Padded to 9 digits
}

TEST(ConvertersTest, TestDateTimeCenturyLeapYear) {
    std::string str = "2000-02-29 12:00:00";  // Year 2000 is a leap year (divisible by 400)
    DateTime dt = strtodatetime(str);

    ASSERT_EQ(2000, dt.year);
    ASSERT_EQ(2, dt.month);
    ASSERT_EQ(29, dt.day);
    ASSERT_EQ(12, dt.hour);
    ASSERT_EQ(0, dt.minute);
    ASSERT_EQ(0, dt.second);
}

TEST(ConvertersTest, TestDateTimeNonLeapCenturyYear) {
    std::string str = "1900-02-29 12:00:00";  // Year 1900 is NOT a leap year (divisible by 100 but not 400)
    ASSERT_THROW(strtodatetime(str), std::invalid_argument);
}

TEST(ConvertersTest, TestDateTimeNanosecondsDotOnly) {
    std::string str = "2024-12-22 14:30:45.";  // Just a dot, no digits (size == 1)
    DateTime dt = strtodatetime(str);
    ASSERT_EQ(0, dt.nanosecond);  // Should default to 0 since size is not > 1
}

TEST(ConvertersTest, TestDateTimeNanosecondsNotStartingWithDot) {
    // Text after time that doesn't start with '.' - should not parse as nanoseconds
    std::string str = "2024-12-22 14:30:45xyz";
    DateTime dt = strtodatetime(str);
    ASSERT_EQ(0, dt.nanosecond);  // Should default to 0 since doesn't start with '.'
}

}  // namespace found
