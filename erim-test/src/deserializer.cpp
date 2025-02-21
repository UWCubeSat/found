#include "deserializer.hpp"
#include <fstream>
#include <iostream>

DataFile deserialize(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for reading: " << filename << std::endl;
        return {};
    }

    DataFile data;

    // Read Magic Number
    file.read(data.magic, sizeof(data.magic));

    // Read Version
    file.read(reinterpret_cast<char*>(&data.version), sizeof(data.version));

    // Read Checksum
    file.read(reinterpret_cast<char*>(&data.checksum), sizeof(data.checksum));

    // Read Coordinate System
    file.read(data.coord_system, sizeof(data.coord_system));

    // Read Number of Positions
    uint32_t numPositions;
    file.read(reinterpret_cast<char*>(&numPositions), sizeof(numPositions));

    // Read Position Vectors
    data.positions.resize(numPositions);
    for (uint32_t i = 0; i < numPositions; i++) {
        file.read(reinterpret_cast<char*>(&data.positions[i]), sizeof(PositionVector));
    }

    file.close();
    return data;
}