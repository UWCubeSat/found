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

}  // namespace found
