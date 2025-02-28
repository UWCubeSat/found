#include <fstream>
#include <iostream>
#include "serialization/serialization.hpp"
#include "spatial/attitude-utils.hpp"

using Vec3 = found::Vec3;

// Calculate CRC32 (example implementation)
uint32_t calculateCRC32(const void* data, size_t length) {
    // Replace this with a proper CRC32 implementation
    uint32_t crc = 0;
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    for (size_t i = 0; i < length; ++i) {
        crc += bytes[i];
    }
    return crc;
}

// Serialize DataFile to a file
void serialize(const DataFile& data, const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Failed to open file for writing");
    }

    // Calculate CRC for the header (excluding the crc field itself)
    DataFileHeader header = data.header;
    header.crc = calculateCRC32(&header, sizeof(header) - sizeof(header.crc));

    // Write the header
    outFile.write(reinterpret_cast<const char*>(&header), sizeof(header));

    // Write the positions
    outFile.write(reinterpret_cast<const char*>(data.positions.data()), data.positions.size() * sizeof(Vec3));
}

// Deserialize DataFile from a file
DataFile deserialize(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Failed to open file for reading");
    }

    DataFile data;

    // Read the header
    inFile.read(reinterpret_cast<char*>(&data.header), sizeof(data.header));

    // Validate the header CRC
    uint32_t expected_crc = calculateCRC32(&data.header, sizeof(data.header) - sizeof(data.header.crc));
    if (data.header.crc != expected_crc) {
        throw std::runtime_error("Header CRC validation failed: Corrupted file");
    }

    // Read the positions
    data.positions.resize(data.header.num_positions);
    inFile.read(reinterpret_cast<char*>(data.positions.data()), data.positions.size() * sizeof(Vec3));

    return data;
}

// Read only the header from a file
DataFileHeader readHeader(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        throw std::runtime_error("Failed to open file for reading");
    }

    DataFileHeader header;
    inFile.read(reinterpret_cast<char*>(&header), sizeof(header));

    // Validate the header CRC
    uint32_t expected_crc = calculateCRC32(&header, sizeof(header) - sizeof(header.crc));
    if (header.crc != expected_crc) {
        throw std::runtime_error("Header CRC validation failed: Corrupted file");
    }

    return header;
}
