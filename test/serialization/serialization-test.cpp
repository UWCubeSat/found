#include <gtest/gtest.h>
#include "serialization/serialization.hpp"
#include <fstream>
#include <cstddef>

namespace found {

class SerializationTest : public ::testing::Test {
protected:
    // Headers in stored (big-endian) format.
    const unsigned char emptyTestHeader[16] = {
        'F', 'O', 'U', 'N', // Magic Number
        0x00U, 0x00U, 0x00U, 0x01U, // Version
        0x00U, 0x00U, 0x00U, 0x00U, // NumPositions
        0x00U, 0x00U, 0x01U, 0x39U // CRC (313 in Decimal)
    };

    const unsigned char incorrectCRCTestHeader[16] = {
        'F', 'O', 'U', 'N', // Magic Number
        0x00U, 0x00U, 0x00U, 0x01U, // Version
        0x00U, 0x00U, 0x00U, 0x00U, // NumPositions
        0x39U, 0x01U, 0x10U, 0x00U // Incorrect CRC
    };

    std::string emptyTestHeaderString;
    std::istringstream emptyTestHeaderStream;

    void SetUp() override {
        emptyTestHeaderString = std::string(reinterpret_cast<const char*>(emptyTestHeader), sizeof(emptyTestHeader));
        emptyTestHeaderStream = std::istringstream(emptyTestHeaderString);
    }

    void TearDown() override {
        // Clean up any test files
        // std::remove("test_data.bin");
    }
};

TEST_F(SerializationTest, CorrectHeader) {
    DataFileHeader header = readHeader(emptyTestHeaderStream);
    ASSERT_EQ(header.magic[0], 'F');
    ASSERT_EQ(header.magic[1], 'O');
    ASSERT_EQ(header.magic[2], 'U');
    ASSERT_EQ(header.magic[3], 'N');
    ASSERT_EQ(header.version, 1U);
    ASSERT_EQ(calculateCRC32(emptyTestHeader, sizeof(header) - sizeof(header.crc)), 313U);
}

TEST_F(SerializationTest, IncorrectHeader) {
    std::string incorrectCRCTestHeaderString;
    std::istringstream incorrectCRCTestHeaderStream;
    incorrectCRCTestHeaderString = std::string(reinterpret_cast<const char*>(incorrectCRCTestHeader), sizeof(incorrectCRCTestHeader));
    incorrectCRCTestHeaderStream = std::istringstream(incorrectCRCTestHeaderString);
    ASSERT_THROW(readHeader(incorrectCRCTestHeaderStream), std::runtime_error);
}

TEST_F(SerializationTest, RoundTripSerialization) {
    // Create a sample DataFile to simulate real data
    DataFile data;

    // Set the header magic and version information
    memcpy(data.header.magic, "FOUN", 4);  // Expected magic number
    data.header.version = 1;
    data.header.num_positions = 2;  // We'll add two positions

    // Set some dummy satellite-relative orientation values
    data.relative_attitude = {123456789, 987654321, 111111111};

    // Add two sample location records with position and timestamp
    LocationRecord loc1;
    loc1.position = {100, 200, 300};
    loc1.timestamp = 161803398;

    found::LocationRecord loc2;
    loc2.position = {-100, -200, -300};
    loc2.timestamp = 271828182;

    data.positions.push_back(loc1);
    data.positions.push_back(loc2);

    // Serialize the DataFile to a memory stream
    std::ostringstream out;
    serialize(data, out);
    std::string buffer = out.str();  // Store binary data as a string

    // Deserialize from the same stream and reconstruct the object
    std::istringstream in(buffer);
    found::DataFile parsed = deserialize(in);

    // Validate the parsed header fields
    ASSERT_EQ(parsed.header.version, 1U);
    ASSERT_EQ(parsed.header.num_positions, 2U);

    // Validate the relative attitude fields
    ASSERT_EQ(parsed.relative_attitude.roll, data.relative_attitude.roll);
    ASSERT_EQ(parsed.relative_attitude.ra, data.relative_attitude.ra);
    ASSERT_EQ(parsed.relative_attitude.de, data.relative_attitude.de);

    // Validate the positions
    ASSERT_EQ(parsed.positions.size(), 2U);
    EXPECT_EQ(parsed.positions[0].timestamp, loc1.timestamp);
    EXPECT_EQ(parsed.positions[0].position.x, loc1.position.x);
    EXPECT_EQ(parsed.positions[1].timestamp, loc2.timestamp);
    EXPECT_EQ(parsed.positions[1].position.y, loc2.position.y);
}

TEST_F(SerializationTest, CorruptedPositionDeserialization) {
    // Create a header that claims to have one position
    std::ostringstream out;
    DataFileHeader header;
    memcpy(header.magic, "FOUN", 4);
    header.version = 1;
    header.num_positions = 1;  // Claims one record

    // Calculate and assign the CRC
    header.crc = found::calculateCRC32(&header, sizeof(header) - sizeof(header.crc));

    // Convert the header fields to network byte order (big endian)
    header.version = htonl(header.version);
    header.num_positions = htonl(header.num_positions);
    header.crc = htonl(header.crc);  // Convert CRC as well

    // Write the header to the output stream
    out.write(reinterpret_cast<const char*>(&header), sizeof(header));

    // Write only 3 bytes of position data (incomplete LocationRecord)
    out.write("bad", 3);  // Simulate a corrupted or truncated file

    std::string buffer = out.str();
    std::istringstream in(buffer);

    // Attempt to deserialize and expect a failure due to corrupted input
    EXPECT_THROW({
        found::deserialize(in);
    }, std::ios_base::failure);
}

TEST_F(SerializationTest, MagicNumberMismatch) {
    // Create a header with an incorrect magic number
    std::ostringstream out;
    DataFileHeader header;
    memcpy(header.magic, "FAIL", 4);  // Wrong magic value
    header.version = 1;
    header.num_positions = 0;

    // Compute CRC based on the incorrect magic
    header.crc = found::calculateCRC32(&header, sizeof(header) - sizeof(header.crc));

    // Convert to network byte order
    header.version = htonl(header.version);
    header.num_positions = htonl(header.num_positions);
    header.crc = htonl(header.crc);

    // Write the faulty header to stream
    out.write(reinterpret_cast<const char*>(&header), sizeof(header));

    std::string buffer = out.str();
    std::istringstream in(buffer);

    // The CRC might pass, but the magic number should trigger a validation failure
    EXPECT_THROW({
        readHeader(in);
    }, std::runtime_error);
}
}  // namespace found
