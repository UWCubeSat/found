#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <string>
#include "serialization/data_file.hpp"  // Include the header for DataFile and DataFileHeader

namespace found {

// Serialize DataFile to a file
void serialize(const DataFile& data, std::ostream& stream);

// Deserialize DataFile from a file
DataFile deserialize(std::istream& stream);

// Read only the header from a file
DataFileHeader readHeader(std::istream& stream);

// Calculate CRC32 for data validation
uint32_t calculateCRC32(const void* data, size_t length);

inline uint16_t htons(uint16_t v) {
    return (v << 8) | (v >> 8);
}

inline uint32_t htonl(uint32_t v) {
    return ((v & 0xFF000000) >> 24) |
           ((v & 0x00FF0000) >> 8) |
           ((v & 0x0000FF00) << 8) |
           ((v & 0x000000FF) << 24);
}

inline uint16_t ntohs(uint16_t v) {
    return (v << 8) | (v >> 8);
}

inline uint32_t ntohl(uint32_t v) {
    return ((v & 0xFF000000) >> 24) |
           ((v & 0x00FF0000) >> 8) |
           ((v & 0x0000FF00) << 8) |
           ((v & 0x000000FF) << 24);
}

// inline uint16_t htons(uint16_t v);
// inline uint32_t htonl(uint32_t v);

// inline uint16_t ntohs(uint16_t v);
// inline uint32_t ntohl(uint32_t v);
}  // namespace found

#endif  // SERIALIZATION_H
