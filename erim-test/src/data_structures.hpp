#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <vector>
#include <cstdint>

// Position Vector (3D)
struct PositionVector {
    float x, y, z;
};

// DataFile format containing metadata and position data
struct DataFile {
    char magic[4] = {'F', 'O', 'U', 'N'}; // File identifier (FOUN)
    uint32_t version = 1;  // File format version
    uint32_t checksum = 0; // Checksum placeholder (to be implemented)
    char coord_system[10]; // "FOUND", "LOST", "GLOBAL"
    std::vector<PositionVector> positions;
};

#endif // DATA_STRUCTURES_H