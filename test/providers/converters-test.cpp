#include <gtest/gtest.h>

#include <getopt.h>

#include <string>

#include "test/common/common.hpp"

#include "src/common/spatial/attitude-utils.hpp"
#include "src/providers/converters.hpp"
#include "src/common/decimal.hpp"

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

TEST(ConvertersTest, TestLocationRecordsNonExistent) {
    ASSERT_THROW(strtolr("not_existent.txt"), std::runtime_error);
}

TEST(ConvertersTest, TestLocationRecordsCSV) {
    ASSERT_THROW(strtolr("test/common/assets/position-data/pos-data-invalid.csv"), std::runtime_error);
}

TEST(ConvertersTest, TestLocationRecordsNormal) {
    LocationRecords records = strtolr("test/common/assets/position-data/pos-data-valid.txt");

    ASSERT_EQ(static_cast<size_t>(3), records.size());
    ASSERT_TRUE(LocationRecordEqual(records[0], {1000000, {1.0, 2.0, 3.0}}));
    ASSERT_TRUE(LocationRecordEqual(records[1], {2000000, {4.0, 5.0, 6.0}}));
    ASSERT_TRUE(LocationRecordEqual(records[2], {3000000, {7.0, 8.0, 9.0}}));
}

}  // namespace found
