#ifndef DATA_FILE_H
#define DATA_FILE_H

#include <vector>
#include <cstdint>
#include <string>
#include "spatial/attitude-utils.hpp"  // Include the header for Vec3


struct DataFileHeader {
    char magic[4] = {'F', 'O', 'U', 'N'};
    uint32_t version = 1;
    uint32_t num_positions;  // Number of positions (Vec3 elements)
    uint32_t crc;  // CRC for header validation

    void validate();
};

struct DataFile {
    DataFileHeader header;
    std::vector<found::Vec3> positions;  // Use Vec3 instead of Position
};

#endif  // DATA_FILE_H
