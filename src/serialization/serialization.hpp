#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <string>
#include "serialization/data_file.hpp"  // Include the header for DataFile and DataFileHeader

namespace found {

// Serialize DataFile to a file
void serialize(const DataFile& data, const std::string& filename);

// Deserialize DataFile from a file
DataFile deserialize(const std::string& filename);

// Read only the header from a file
DataFileHeader readHeader(const std::string& filename);

// Calculate CRC32 for data validation
uint32_t calculateCRC32(const void* data, size_t length);

}  // namespace found

#endif  // SERIALIZATION_H
