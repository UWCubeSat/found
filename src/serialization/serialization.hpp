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
inline uint16_t htons(uint16_t v) {
    return (v << 8) | (v >> 8);
}

/**
 * @brief Converts a 32-bit integer from host byte order to network byte order.
 */
inline uint32_t htonl(uint32_t v) {
    return ((v & 0xFF000000) >> 24) |
            ((v & 0x00FF0000) >> 8) |
            ((v & 0x0000FF00) << 8) |
            ((v & 0x000000FF) << 24);
}

/**
 * @brief Converts a 64-bit integer from host byte order to network byte order.
 */
inline uint64_t htonl(uint64_t v) {
    return ((v & 0xFF00000000000000ULL) >> 56) |
           ((v & 0x00FF000000000000ULL) >> 40) |
           ((v & 0x0000FF0000000000ULL) >> 24) |
           ((v & 0x000000FF00000000ULL) >> 8) |
           ((v & 0x00000000FF000000ULL) << 8) |
           ((v & 0x0000000000FF0000ULL) << 24) |
           ((v & 0x000000000000FF00ULL) << 40) |
           ((v & 0x00000000000000FFULL) << 56);
}

/**
 * @brief Converts a 16-bit integer from network byte order to host byte order.
 */
inline uint16_t ntohs(uint16_t v) {
    return (v << 8) | (v >> 8);
}

/**
 * @brief Converts a 32-bit integer from network byte order to host byte order.
 */
inline uint32_t ntohl(uint32_t v) {
    return ((v & 0xFF000000) >> 24) |
            ((v & 0x00FF0000) >> 8) |
            ((v & 0x0000FF00) << 8) |
            ((v & 0x000000FF) << 24);
}

/**
 * @brief Converts a 64-bit integer from network byte order to host byte order.
 */
inline uint64_t ntohl(uint64_t v) {
    return ((v & 0xFF00000000000000ULL) >> 56) |
           ((v & 0x00FF000000000000ULL) >> 40) |
           ((v & 0x0000FF0000000000ULL) >> 24) |
           ((v & 0x000000FF00000000ULL) >> 8) |
           ((v & 0x00000000FF000000ULL) << 8) |
           ((v & 0x0000000000FF0000ULL) << 24) |
           ((v & 0x000000000000FF00ULL) << 40) |
           ((v & 0x00000000000000FFULL) << 56);
}

inline float ntoh_f(float v) {
    union _f_ {
        float f;
        uint32_t u;
    };
    _f_ temp;
    temp.f = v;
    temp.u = ntohl(temp.u);
    return temp.f;
}

inline double ntoh_d(double v) {
    union _d_ {
        double d;
        uint64_t u;
    };
    _d_ temp;
    temp.d = v;
    temp.u = ntohl(temp.u);
    return temp.d;
}

inline float hton_f(float v) {
    union _f_ {
        float f;
        uint32_t u;
    };
    _f_ temp;
    temp.f = v;
    temp.u = htonl(temp.u);
    return temp.f;
}

inline double hton_d(double v) {
    union _d_ {
        double d;
        uint64_t u;
    };
    _d_ temp;
    temp.d = v;
    temp.u = htonl(temp.u);
    return temp.d;
}

/**
 * @brief Converts a decimal from host byte order to network byte order.
 * 
 * This function is a wrapper around htonl for decimal types.
 * 
 * @param v The decimal value to convert.
 * @return The converted decimal value in network byte order.
 */
inline decimal htond(decimal v) {
#if FOUND_FLOAT_MODE
    return hton_f(v);
#else
    return hton_d(v);
#endif
}

/**
 * @brief Converts a decimal from network byte order to host byte order.
 * 
 * This function is a wrapper around ntohl for decimal types.
 * 
 * @param v The decimal value to convert.
 * @return The converted decimal value in host byte order.
 */
inline decimal ntohd(decimal v) {
#if FOUND_FLOAT_MODE
    return hton_f(v);
#else
    return hton_d(v);
#endif
}

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
