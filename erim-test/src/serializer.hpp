// #ifndef SERIALIZER_H
// #define SERIALIZER_H

// #include "data_structures.hpp"
// #include <string>

// void serialize(const DataFile& data, const std::string& filename);

// #endif // SERIALIZER_H



// #ifndef SERIALIZER_H
// #define SERIALIZER_H

// #include "data_structures.hpp"
// #include <string>
// #include <fstream>

// // Serialize DataFile to a file
// void serialize(const DataFile& data, const std::string& filename);

// // Deserialize DataFile from a file
// DataFile deserialize(const std::string& filename);

// // Read only the header from a file
// DataFileHeader readHeader(const std::string& filename);

// #endif // SERIALIZER_H


#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include "data_structures.hpp"
#include <string>

// Serialize DataFile to a file
void serialize(const DataFile& data, const std::string& filename);

// Deserialize DataFile from a file
DataFile deserialize(const std::string& filename);

// Read only the header from a file
DataFileHeader readHeader(const std::string& filename);

#endif // SERIALIZER_HPP