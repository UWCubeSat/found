#ifndef SRC_DATAFILE_SERIALIZATION_HPP_
#define SRC_DATAFILE_SERIALIZATION_HPP_

#include <string>

#include "datafile/datafile.hpp"  // Include the header for DataFile and DataFileHeader

namespace found {

/**
 * @brief Serializes a DataFile object to an output stream.
 * 
 * @param data The DataFile object to serialize.
 * @param stream The output stream to write the serialized data to.
 * 
 * @pre The number of positions in the header must match the number of positions in the entry.
 */
void serializeDataFile(const DataFile& data, std::ostream& stream);

/**
 * @brief Deserializes a DataFile object from an input stream.
 * 
 * @param stream The input stream to read the serialized data from.
 * 
 * @return The deserialized DataFile object.
 */
DataFile deserializeDataFile(std::istream& stream);

/**
 * @brief Deserializes a DataFile object from an input stream.
 * 
 * @param stream The input stream to read the serialized data from.
 * @param path The path to associate the resulting struct with
 * 
 * @return The deserialized DataFile object, with the given path.
 */
DataFile deserializeDataFile(std::istream& stream, const std::string &path);

/**
 * @brief Reads only the header of a DataFile from an input stream.
 * 
 * @param stream The input stream to read the header from.
 * 
 * @return The DataFileHeader object containing the header information.
 */
DataFileHeader readHeader(std::istream& stream);

/**
 * @brief Converts a DataFileHeader from host to network byte order.
 * 
 * @param header The header to convert.
 * 
 * @post header's fields are in network byte order.
 */
void hton(DataFileHeader& header);

/**
 * @brief Converts a DataFileHeader from network to host byte order.
 * 
 * @param header The header to convert.
 * 
 * @post header's fields are in host byte order.
 */
void ntoh(DataFileHeader& header);

}  // namespace found

#endif  // SRC_DATAFILE_SERIALIZATION_HPP_
