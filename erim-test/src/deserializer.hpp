#ifndef DESERIALIZER_HPP
#define DESERIALIZER_HPP

#include "data_structures.hpp"
#include <string>

// Deserialize DataFile from a file
DataFile deserialize(const std::string& filename);

// Read only the header from a file
DataFileHeader readHeader(const std::string& filename);

#endif // DESERIALIZER_HPP