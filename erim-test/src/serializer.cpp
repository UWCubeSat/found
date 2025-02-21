#include "serializer.hpp"
#include <fstream>
#include <iostream>

void serialize(const DataFile& data, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    // Write Magic Number
    file.write(data.magic, sizeof(data.magic));

    // Write Version
    file.write(reinterpret_cast<const char*>(&data.version), sizeof(data.version));

    // Write Checksum (dummy value for now)
    file.write(reinterpret_cast<const char*>(&data.checksum), sizeof(data.checksum));

    // Write Coordinate System
    file.write(data.coord_system, sizeof(data.coord_system));

    // Write Number of Positions
    uint32_t numPositions = data.positions.size();
    file.write(reinterpret_cast<const char*>(&numPositions), sizeof(numPositions));

    // Write Position Vectors
    for (const auto& pos : data.positions) {
        file.write(reinterpret_cast<const char*>(&pos), sizeof(PositionVector));
    }

    file.close();
}