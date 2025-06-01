#include <fstream>
#include <sstream>
#include <cstddef>
#include <gtest/gtest.h>

#include "serialization/encoding.hpp"
#include "serialization/serialization.hpp"

namespace found {

/**
 * @class SerializationTest
 * @brief Unit test fixture for serialization/deserialization functions.
 */
class SerializationTest : public ::testing::Test {
protected:
    /**
     * @brief A valid DataFileHeader in network byte order for an empty file (0 positions).
     * CRC is set to 313 (0x0139), calculated from the first 12 bytes.
     */
    const unsigned char emptyTestHeader[16] = {
        'F', 'O', 'U', 'N',             // Magic Number
        0x00U, 0x00U, 0x00U, 0x01U,     // Version = 1
        0x00U, 0x00U, 0x00U, 0x00U,     // NumPositions = 0
        0x95U, 0x36U, 0x85U, 0x7FU      // CRC = 2503378303 (0x9536857F)
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
    ASSERT_EQ(calculateCRC32(emptyTestHeader, sizeof(DataFileHeader) - sizeof(uint32_t)), 2503378303U);
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
 * @test Serializes and deserializes a DataFile object and verifies round-trip consistency.
 */
TEST_F(SerializationTest, RoundTripSerialization) {
    DataFile data;
    memcpy(data.header.magic, "FOUN", 4);
    data.header.version = 1;
    data.header.num_positions = 2;
    data.relative_attitude = {123456789, 987654321, 111111111};

    LocationRecord loc1{{100, 200, 300}, 161803398};
    LocationRecord loc2{{-100, -200, -300}, 271828182};

    data.positions.push_back(loc1);
    data.positions.push_back(loc2);

    std::ostringstream out;
    serialize(data, out);
    std::string buffer = out.str();

    std::istringstream in(buffer);
    DataFile parsed = deserialize(in);

    ASSERT_EQ(parsed.header.version, 1U);
    ASSERT_EQ(parsed.header.num_positions, 2U);

    ASSERT_EQ(parsed.relative_attitude.roll, data.relative_attitude.roll);
    ASSERT_EQ(parsed.relative_attitude.ra, data.relative_attitude.ra);
    ASSERT_EQ(parsed.relative_attitude.de, data.relative_attitude.de);

    ASSERT_EQ(parsed.positions.size(), 2U);
    EXPECT_EQ(parsed.positions[0].timestamp, loc1.timestamp);
    EXPECT_EQ(parsed.positions[0].position.x, loc1.position.x);
    EXPECT_EQ(parsed.positions[1].timestamp, loc2.timestamp);
    EXPECT_EQ(parsed.positions[1].position.y, loc2.position.y);
}

/**
 * @test Simulates a corrupted input stream by writing partial position data and expects failure.
 */
TEST_F(SerializationTest, CorruptedPositionDeserialization) {
    std::ostringstream out;
    DataFileHeader header;
    memcpy(header.magic, "FOUN", 4);
    header.version = 1;
    header.num_positions = 1;
    header.version = htonl(header.version);
    header.num_positions = htonl(header.num_positions);

    header.crc = found::calculateCRC32(&header, sizeof(header) - sizeof(header.crc));
    header.crc = htonl(header.crc);

    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    out.write("bad", 3);  // Incomplete LocationRecord

    std::string buffer = out.str();
    std::istringstream in(buffer);

    EXPECT_THROW({
        found::deserialize(in);
    }, std::ios_base::failure);
}

/**
 * @test Provides a header with an invalid magic number and expects validation failure.
 */
TEST_F(SerializationTest, MagicNumberMismatch) {
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
 * @test Serializes and deserializes a DataFile object with zero positions.
 */
TEST_F(SerializationTest, RoundTripSerializationEmptyPositions) {
    DataFile data;
    memcpy(data.header.magic, "FOUN", 4);
    data.header.version = 1;
    data.header.num_positions = 0;
    data.relative_attitude = {0, 0, 0};
    // No positions added

    std::ostringstream out;
    serialize(data, out);
    std::string buffer = out.str();

    std::istringstream in(buffer);
    DataFile parsed = deserialize(in);

    ASSERT_EQ(parsed.header.version, 1U);
    ASSERT_EQ(parsed.header.num_positions, 0U);
    ASSERT_EQ(parsed.relative_attitude.roll, 0);
    ASSERT_EQ(parsed.relative_attitude.ra, 0);
    ASSERT_EQ(parsed.relative_attitude.de, 0);
    ASSERT_TRUE(parsed.positions.empty());
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
    header.version = htonl(header.version);
    header.num_positions = htonl(header.num_positions);
    header.crc = found::calculateCRC32(&header, sizeof(header) - sizeof(header.crc));
    header.crc = htonl(header.crc);

    std::ostringstream out;
    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    std::string buffer = out.str();
    std::istringstream in(buffer);

    // Should throw because relative_attitude is missing
    EXPECT_THROW({
        found::deserialize(in);
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
    header.num_positions = 1; // Add a position which is missing.
    header.version = htonl(header.version);
    header.num_positions = htonl(header.num_positions);
    header.crc = found::calculateCRC32(&header, sizeof(header) - sizeof(header.crc));
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
        found::deserialize(in);
    }, std::ios_base::failure);
}

}  // namespace found
