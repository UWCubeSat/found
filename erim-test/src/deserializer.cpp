#include "deserializer.hpp"
#include <fstream>
#include <iostream>

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
    inFile.read(reinterpret_cast<char*>(data.positions.data()), data.positions.size() * sizeof(Position));

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