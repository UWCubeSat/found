#include <gtest/gtest.h>
#include <memory>
#include <cstddef>
#include <fstream>
#include <sstream>
#include <string>

#include "test/common/common.hpp"

#include "src/common/decimal.hpp"

#include "src/datafile/encoding.hpp"
#include "src/datafile/serialization.hpp"

namespace found {

/**
 * @class SerializationTest
 * @brief Unit test fixture for serialization/deserialization functions.
 */
class SerializationTest : public ::testing::Test {
 protected:
    /**
     * @brief A very invalid DataFileHeader
     */
    const std::string nonsenseHeader = "blablabla";

    /**
     * @brief A valid DataFileHeader in network byte order for an empty file (0 positions).
     * CRC is set to 1694280785 (0x64FCAC51), calculated from the first 12 bytes.
     */
    const unsigned char emptyTestHeader[16] = {
        'F', 'O', 'U', 'N',             // Magic Number
        0x00U, 0x00U, 0x00U, 0x01U,     // Version = 1
        0x00U, 0x00U, 0x00U, 0x00U,     // NumPositions = 0
        0x64U, 0xFCU, 0xACU, 0x51U      // CRC = 1694280785 (0x64FCAC51)
    };

    /**
     * @brief A header with a deliberately incorrect CRC value.
     */
    const unsigned char incorrectCRCTestHeader[16] = {
        'F', 'O', 'U', 'N',             // Magic Number
        0x00U, 0x00U, 0x00U, 0x01U,     // Version = 1
        0x00U, 0x00U, 0x00U, 0x00U,     // NumPositions = 0
        0x39U, 0x01U, 0x10U, 0x00U      // Incorrect CRC
    };

    std::string emptyTestHeaderString;
    std::istringstream emptyTestHeaderStream;

    void SetUp() override {
        emptyTestHeaderString = std::string(reinterpret_cast<const char*>(emptyTestHeader), sizeof(emptyTestHeader));
        emptyTestHeaderStream = std::istringstream(emptyTestHeaderString);
    }

    void TearDown() override {
        // Currently unused, kept for future file cleanup if needed
    }
};

/**
 * @test Verifies that a correctly constructed header is parsed successfully.
 */
TEST_F(SerializationTest, CorrectHeader) {
    DataFileHeader header = readHeader(emptyTestHeaderStream);
    ASSERT_EQ(header.magic[0], 'F');
    ASSERT_EQ(header.magic[1], 'O');
    ASSERT_EQ(header.magic[2], 'U');
    ASSERT_EQ(header.magic[3], 'N');
    ASSERT_EQ(header.version, 1U);
    ASSERT_EQ(calculateCRC32(
        reinterpret_cast<const char*>(&header),
        sizeof(DataFileHeader) - sizeof(uint32_t)),
        1694280785U);
}

/**
 * @test Ensures that an incorrect CRC value triggers a runtime error.
 */
TEST_F(SerializationTest, IncorrectHeader) {
    std::string incorrectStr(reinterpret_cast<const char*>(incorrectCRCTestHeader), sizeof(incorrectCRCTestHeader));
    std::istringstream stream(incorrectStr);
    ASSERT_THROW(readHeader(stream), std::runtime_error);
}

/**
 * @test Ensures that an incorrect CRC value triggers a runtime error.
 */
TEST_F(SerializationTest, IncorrectSizeHeader) {
    std::string incorrectStr(nonsenseHeader.c_str(), nonsenseHeader.size());
    std::istringstream stream(incorrectStr);
    ASSERT_THROW(readHeader(stream), std::runtime_error);
}

/**
 * @test Serializes and deserializes a DataFile object and verifies round-trip consistency.
 */
TEST_F(SerializationTest, RoundTripSerialization) {
    DataFile expected;
    memcpy(expected.header.magic, "FOUN", 4);
    expected.header.version = 1;
    expected.header.num_positions = 2;
    expected.relative_attitude = {0, 123456789., 987654321., 111111111.};

    LocationRecord loc1{161803398, {100, 200, 300}};
    LocationRecord loc2{271828182, {-100, -200, -300}};

    expected.positions = std::make_unique<LocationRecord[]>(2);
    expected.positions[0] = loc1;
    expected.positions[1] = loc2;

    std::ostringstream out;
    serializeDataFile(expected, out);
    std::string buffer = out.str();

    std::istringstream in(buffer);
    DataFile actual = deserializeDataFile(in);

    ASSERT_DF_EQ_DEFAULT(expected, actual);
}

/**
 * @test Simulates a corrupted input stream by writing partial position data and expects failure.
 */
TEST_F(SerializationTest, CorruptedPositionDeserialization1) {
    std::ostringstream out;
    DataFileHeader header;
    memcpy(header.magic, "FOUN", 4);
    header.version = 1;
    header.num_positions = 1;
    header.crc = found::calculateCRC32(&header, sizeof(header) - sizeof(header.crc));
    header.version = htonl(header.version);
    header.num_positions = htonl(header.num_positions);
    header.crc = htonl(header.crc);
    out.write(reinterpret_cast<const char*>(&header), sizeof(header));

    // Incomplete LocationRecord, specifically incomplete relative attitude
    out.write("bad", 3);

    std::string buffer = out.str();
    std::istringstream in(buffer);

    EXPECT_THROW({
        found::deserializeDataFile(in);
    }, std::ios_base::failure);
}

/**
 * @test Simulates a corrupted input stream by writing partial position data and expects failure.
 */
TEST_F(SerializationTest, CorruptedPositionDeserialization2) {
    std::ostringstream out;
    DataFileHeader header;
    memcpy(header.magic, "FOUN", 4);
    header.version = 1;
    header.num_positions = 1;
    header.crc = found::calculateCRC32(&header, sizeof(header) - sizeof(header.crc));
    header.version = htonl(header.version);
    header.num_positions = htonl(header.num_positions);
    header.crc = htonl(header.crc);
    out.write(reinterpret_cast<const char*>(&header), sizeof(header));

    // Now write a quaternion in file format (all fields are double, so that it works for FOUND_FLOAT_MODE too)
    struct Quat {
        double real = 1.2;
        double i = -2.16;
        double j = 0.6;
        double k = -0.2;
    } quaternion_field;
    out.write(reinterpret_cast<const char *>(&quaternion_field), sizeof(quaternion_field));

    // Now write a location record, but only part of the first field
    LocationRecord locationRecord = {1000, {1, 2, 3}};
    out.write(reinterpret_cast<const char *>(&locationRecord.position), 1);  // Incomplete LocationRecord

    std::string buffer = out.str();
    std::istringstream in(buffer);

    EXPECT_THROW({
        found::deserializeDataFile(in);
    }, std::ios_base::failure);
}

/**
 * @test Provides a header with an invalid magic number and expects validation failure.
 */
TEST_F(SerializationTest, MagicNumberMismatch1) {
    std::ostringstream out;
    DataFileHeader header;
    memcpy(header.magic, "MEEP", 4);  // Invalid magic
    header.version = 1;
    header.num_positions = 0;

    header.version = htonl(header.version);
    header.num_positions = htonl(header.num_positions);
    header.crc = found::calculateCRC32(&header, sizeof(header) - sizeof(header.crc));
    header.crc = htonl(header.crc);

    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    std::string buffer = out.str();
    std::istringstream in(buffer);

    EXPECT_THROW(readHeader(in), std::runtime_error);
}

/**
 * @test Provides a header with an invalid magic number and expects validation failure.
 */
TEST_F(SerializationTest, MagicNumberMismatch2) {
    std::ostringstream out;
    DataFileHeader header;
    memcpy(header.magic, "FAIL", 4);  // Invalid magic
    header.version = 1;
    header.num_positions = 0;

    header.version = htonl(header.version);
    header.num_positions = htonl(header.num_positions);
    header.crc = found::calculateCRC32(&header, sizeof(header) - sizeof(header.crc));
    header.crc = htonl(header.crc);

    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    std::string buffer = out.str();
    std::istringstream in(buffer);

    EXPECT_THROW(readHeader(in), std::runtime_error);
}

/**
 * @test Provides a header with an invalid magic number and expects validation failure.
 */
TEST_F(SerializationTest, MagicNumberMismatch3) {
    std::ostringstream out;
    DataFileHeader header;
    memcpy(header.magic, "FOIL", 4);  // Invalid magic
    header.version = 1;
    header.num_positions = 0;

    header.version = htonl(header.version);
    header.num_positions = htonl(header.num_positions);
    header.crc = found::calculateCRC32(&header, sizeof(header) - sizeof(header.crc));
    header.crc = htonl(header.crc);

    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    std::string buffer = out.str();
    std::istringstream in(buffer);

    EXPECT_THROW(readHeader(in), std::runtime_error);
}

/**
 * @test Provides a header with an invalid magic number and expects validation failure.
 */
TEST_F(SerializationTest, MagicNumberMismatch4) {
    std::ostringstream out;
    DataFileHeader header;
    memcpy(header.magic, "FOUL", 4);  // Invalid magic
    header.version = 1;
    header.num_positions = 0;

    header.version = htonl(header.version);
    header.num_positions = htonl(header.num_positions);
    header.crc = found::calculateCRC32(&header, sizeof(header) - sizeof(header.crc));
    header.crc = htonl(header.crc);

    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    std::string buffer = out.str();
    std::istringstream in(buffer);

    EXPECT_THROW(readHeader(in), std::runtime_error);
}

/**
 * @test Serializes and deserializes a DataFile object with zero positions.
 */
TEST_F(SerializationTest, RoundTripSerializationEmptyPositions) {
    DataFile data;
    memcpy(data.header.magic, "FOUN", 4);
    data.header.version = 1;
    data.header.num_positions = 0;
    data.relative_attitude = {0, 0, 0, 0};
    // No positions added

    std::ostringstream out;
    serializeDataFile(data, out);
    std::string buffer = out.str();

    std::istringstream in(buffer);
    DataFile parsed = deserializeDataFile(in);

    ASSERT_DF_EQ_DEFAULT(data, parsed);
}

/**
 * @test Deserializes a file that contains only a valid header and nothing else.
 * Should fail because RelativeAttitude is missing.
 */
TEST_F(SerializationTest, OnlyHeaderNoRelativeAttitude) {
    // Prepare a header with num_positions = 0
    DataFileHeader header;
    memcpy(header.magic, "FOUN", 4);
    header.version = 1;
    header.num_positions = 0;
    header.crc = found::calculateCRC32(&header, sizeof(header) - sizeof(header.crc));
    header.version = htonl(header.version);
    header.num_positions = htonl(header.num_positions);
    header.crc = htonl(header.crc);

    std::ostringstream out;
    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    std::string buffer = out.str();
    std::istringstream in(buffer);

    // Should throw because relative_attitude is missing
    EXPECT_THROW({
        found::deserializeDataFile(in);
    }, std::ios_base::failure);
}

/**
 * @test Deserializes a file that contains invalid number of positions
 */
TEST_F(SerializationTest, MissingPositions) {
    // Prepare a header with num_positions = 0
    DataFileHeader header;
    memcpy(header.magic, "FOUN", 4);
    header.version = 1;
    header.num_positions = 1;  // Add a position which is missing.
    header.crc = found::calculateCRC32(&header, sizeof(header) - sizeof(header.crc));
    header.version = htonl(header.version);
    header.num_positions = htonl(header.num_positions);
    header.crc = htonl(header.crc);

    std::ostringstream out;
    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    // Write an empty relative_attitude, which is required
    EulerAngles angles;
    out.write(reinterpret_cast<const char*>(&angles), sizeof(EulerAngles));
    std::string buffer = out.str();
    std::istringstream in(buffer);

    // Should throw because relative_attitude is missing
    EXPECT_THROW({
        found::deserializeDataFile(in);
    }, std::ios_base::failure);
}

}  // namespace found
