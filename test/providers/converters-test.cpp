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

TEST(ConvertersTest, TestEAExtraValuesIgnored) {
    std::string str = "10,20,30,40";
    EulerAngles angles = strtoea(str);

    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(DegToRad(10)), angles.ra);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(DegToRad(20)), angles.de);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(DegToRad(30)), angles.roll);
}

TEST(ConvertersTest, TestEASingleValuePadsZeroes) {
    std::string str = "5";
    EulerAngles angles = strtoea(str);

    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(DegToRad(5)), angles.ra);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), angles.de);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), angles.roll);
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
    DataFile expected{{{'F', 'O', 'U', 'N'}, 1U, 5}, Quaternion(-0.26, 8.5, 0, 9.2)};
    expected.positions.push_back({45, {95.21, -62.15, 62.14}});
    expected.positions.push_back({62, {623.25, -6182.9, -361.2}});
    expected.positions.push_back({821, {623.26, 86.18, -105.21}});
    expected.positions.push_back({926, {156.16, -296.29, 682.21}});
    expected.positions.push_back({1062, {61.16, -168.21, -181.21}});
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
    DataFile expected{{{'F', 'O', 'U', 'N'}, 1U, 3}, Quaternion(1, 0, 0, 0)};
    expected.positions.push_back({45, {95.21, -62.15, 62.14}});
    expected.positions.push_back({62, {623.25, -6182.9, -361.2}});
    expected.positions.push_back({821, {623.26, 86.18, -105.21}});
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

TEST(ConvertersTest, TestLocationRecordsDataFileEmpty) {
    DataFile expected{{{'F', 'O', 'U', 'N'}, 1U, 0}, Quaternion(1, 0, 0, 0)};
    std::ofstream file(temp_df);
    serializeDataFile(expected, file);
    file.flush();
    LocationRecords actual = strtolr(temp_df);

    ASSERT_EQ(static_cast<size_t>(0), actual.size());

    std::remove(temp_df);
}

TEST(ConvertersTest, TestLocationRecordsEmptyTextFile) {
    const std::string path = "test/common/assets/temp-empty-pos-data.txt";
    std::ofstream file(path);
    file.close();

    LocationRecords actual = strtolr(path);
    ASSERT_TRUE(actual.empty());

    std::remove(path.c_str());
}

TEST(ConvertersTest, TestLocationRecordsTooManyLines) {
    const std::string path = "test/common/assets/temp-pos-data.txt";
    std::ofstream file(path);
    for (size_t i = 0; i <= FOUND_MAX_LOCATION_RECORDS; ++i) {
        file << i << " 1 2 3\n";
    }
    file.close();

    ASSERT_THROW(strtolr(path), std::runtime_error);

    std::remove(path.c_str());
}

}  // namespace found
