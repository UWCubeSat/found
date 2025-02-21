// #ifndef DATA_STRUCTURES_H
// #define DATA_STRUCTURES_H

// #include <vector>
// #include <cstdint>

// // Position Vector (3D)
// struct PositionVector {
//     float x, y, z;
// };

// // DataFile format containing metadata and position data
// struct DataFile {
//     char magic[4] = {'F', 'O', 'U', 'N'}; // File identifier (FOUN)
//     uint32_t version = 1;  // File format version
//     uint32_t checksum = 0; // Checksum placeholder (to be implemented)
//     char coord_system[10]; // "FOUND", "LOST", "GLOBAL"
//     std::vector<PositionVector> positions;
// };

// #endif // DATA_STRUCTURES_H


#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <vector>
#include <cstdint>
#include <string>

struct Position {
    double x, y, z;
};

struct DataFileHeader {
    char coord_system[32];
    uint32_t num_positions;
    uint32_t crc; // CRC for header validation
};

struct DataFile {
    DataFileHeader header;
    std::vector<Position> positions;
};

// Calculate CRC32 for data validation
uint32_t calculateCRC32(const void* data, size_t length);

#endif // DATA_STRUCTURES_H