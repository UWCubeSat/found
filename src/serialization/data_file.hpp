#ifndef DATA_FILE_H
#define DATA_FILE_H

#include <vector>
#include <cstdint>
#include <string>
#include "spatial/attitude-utils.hpp"  // Include the header for Vec3

namespace found {

struct DataFileHeader {
    char magic[4] = {'F', 'O', 'U', 'N'};
    uint32_t version = 1;
    uint32_t num_positions;  // Number of positions (Vec3 elements)
    uint32_t crc;  // CRC for header validation

    void validate();
};

struct LocationRecord {
    Vec3 position;
    uint64_t timestamp;
};

struct DataFile {
    DataFileHeader header;
    EulerAngles relative_attitude = EulerAngles(0, 0, 0);  // Attitude of the object
    std::vector<LocationRecord> positions;
};

}  // namespace found

#endif  // DATA_FILE_H
