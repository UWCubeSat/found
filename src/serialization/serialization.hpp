#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <string>
#include "serialization/data_file.hpp"  // Include the header for DataFile and DataFileHeader

namespace found {

    /**
     * @brief Serializes a DataFile object to an output stream.
     * 
     * @param data The DataFile object to serialize.
     * @param stream The output stream to write the serialized data to.
     */
    void serialize(const DataFile& data, std::ostream& stream);

    /**
     * @brief Deserializes a DataFile object from an input stream.
     * 
     * @param stream The input stream to read the serialized data from.
     * @return The deserialized DataFile object.
     */
    DataFile deserialize(std::istream& stream);

    /**
     * @brief Reads only the header of a DataFile from an input stream.
     * 
     * @param stream The input stream to read the header from.
     * @return The DataFileHeader object containing the header information.
     */
    DataFileHeader readHeader(std::istream& stream);

    /**
     * @brief Calculates the CRC32 checksum for a given data buffer.
     * 
     * @param data Pointer to the data buffer.
     * @param length The size of the data buffer in bytes.
     * @return The calculated CRC32 checksum.
     */
    uint32_t calculateCRC32(const void* data, size_t length);

    /**
     * @brief Converts a 16-bit integer from host byte order to network byte order.
     */
    inline uint16_t htons(uint16_t v);

    /**
     * @brief Converts a 32-bit integer from host byte order to network byte order.
     */
    inline uint32_t htonl(uint32_t v);

    /**
     * @brief Converts a 16-bit integer from network byte order to host byte order.
     */
    inline uint16_t ntohs(uint16_t v);

    /**
     * @brief Converts a 32-bit integer from network byte order to host byte order.
     */
    inline uint32_t ntohl(uint32_t v);

    /**
     * @brief Converts a DataFileHeader from host to network byte order.
     * @param header The header to convert.
     */
    void hton(DataFileHeader& header);

    /**
     * @brief Converts EulerAngles from host to network byte order.
     * @param angles The EulerAngles to convert.
     */
    void hton(EulerAngles& angles);

    /**
     * @brief Converts a Vec3 from host to network byte order.
     * @param v The Vec3 to convert.
     */
    void hton(Vec3& v);

    /**
     * @brief Converts a DataFileHeader from network to host byte order.
     * @param header The header to convert.
     */
    void ntoh(DataFileHeader& header);

    /**
     * @brief Converts EulerAngles from network to host byte order.
     * @param angles The EulerAngles to convert.
     */
    void ntoh(EulerAngles& angles);

    /**
     * @brief Converts a Vec3 from network to host byte order.
     * @param v The Vec3 to convert.
     */
    void ntoh(Vec3& v);

}  // namespace found

#endif  // SERIALIZATION_H
