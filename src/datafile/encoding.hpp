#ifndef SRC_DATAFILE_ENCODING_HPP_
#define SRC_DATAFILE_ENCODING_HPP_

#include <endian.h>
#include <stdint.h>

#include "common/decimal.hpp"

#ifdef __BYTE_ORDER__
    #define ENDIANESS __BYTE_ORDER__
#else
    // manual definition
    #define ENDIANESS __ORDER_LITTLE_ENDIAN__
#endif

namespace found {

/**
 * @brief Converts a 16-bit integer from host byte order to network byte order.
 * 
 * @param v The integer to convert
 * 
 * @return The integer in network byte order.
 */
inline uint16_t htons(uint16_t v) {
#if ENDIANESS == __ORDER_LITTLE_ENDIAN__
    return (v << 8) | (v >> 8);
#else
    return v;
#endif
}

/**
 * @brief Converts a 16-bit integer from network byte order to host byte order.
 * 
 * @param v The integer to convert
 * 
 * @return The integer in host byte order.
 */
inline uint16_t ntohs(uint16_t v) {
    #if ENDIANESS == __ORDER_LITTLE_ENDIAN__
        return (v << 8) | (v >> 8);
    #else
        return v;
    #endif
}

/**
 * @brief Converts a 32-bit integer from host byte order to network byte order.
 * 
 * @param v The integer to convert
 * 
 * @return The integer in network byte order.
 */
inline uint32_t htonl(uint32_t v) {
    #if ENDIANESS == __ORDER_LITTLE_ENDIAN__
        return ((v & 0xFF000000) >> 24) |
               ((v & 0x00FF0000) >> 8) |
               ((v & 0x0000FF00) << 8) |
               ((v & 0x000000FF) << 24);
    #else
        return v;
    #endif
}

/**
 * @brief Converts a 32-bit integer from network byte order to host byte order.
 * 
 * @param v The integer to convert
 * 
 * @return The integer in host byte order.
 */
inline uint32_t ntohl(uint32_t v) {
    #if ENDIANESS == __ORDER_LITTLE_ENDIAN__
        return ((v & 0xFF000000) >> 24) |
               ((v & 0x00FF0000) >> 8) |
               ((v & 0x0000FF00) << 8) |
               ((v & 0x000000FF) << 24);
    #else
        return v;
    #endif
}

/**
 * @brief Converts a 64-bit integer from host byte order to network byte order.
 * 
 * @param v The integer to convert
 * 
 * @return The integer in network byte order.
 */
inline uint64_t htonl(uint64_t v) {
    #if ENDIANESS == __ORDER_LITTLE_ENDIAN__
        return ((v & 0xFF00000000000000ULL) >> 56) |
               ((v & 0x00FF000000000000ULL) >> 40) |
               ((v & 0x0000FF0000000000ULL) >> 24) |
               ((v & 0x000000FF00000000ULL) >> 8) |
               ((v & 0x00000000FF000000ULL) << 8) |
               ((v & 0x0000000000FF0000ULL) << 24) |
               ((v & 0x000000000000FF00ULL) << 40) |
               ((v & 0x00000000000000FFULL) << 56);
    #else
        return v;
    #endif
}

/**
 * @brief Converts a 64-bit integer from network byte order to host byte order.
 * 
 * @param v The integer to convert
 * 
 * @return The integer in host byte order.
 */
inline uint64_t ntohl(uint64_t v) {
    #if ENDIANESS == __ORDER_LITTLE_ENDIAN__
        return ((v & 0xFF00000000000000ULL) >> 56) |
               ((v & 0x00FF000000000000ULL) >> 40) |
               ((v & 0x0000FF0000000000ULL) >> 24) |
               ((v & 0x000000FF00000000ULL) >> 8) |
               ((v & 0x00000000FF000000ULL) << 8) |
               ((v & 0x0000000000FF0000ULL) << 24) |
               ((v & 0x000000000000FF00ULL) << 40) |
               ((v & 0x00000000000000FFULL) << 56);
    #else
        return v;
    #endif
}

/**
 * @brief Union for converting a 32-bit floating point number from host byte order to network byte order and vice versa.
 */
union _f_u_ {
    /// @brief Floating-point variable used for storing a single-precision value.
    float f;
    /// @brief Unsigned 32-bit integer used for encoding or serialization purposes.
    uint32_t u;
};

/**
 * @brief Union for converting a 64-bit floating point number from host byte order to network byte order and vice versa.
 */
union _d_u_ {
    /// @brief A variable to store a double-precision floating-point value.
    double d;
    /// @brief Unsigned 64-bit integer used for encoding or serialization purposes.
    uint64_t u;
};

/**
 * @brief Converts a float from network byte order to host byte order.
 * 
 * @param v The float value to convert.
 * @return The converted float value in host byte order.
 */
inline float htonf(float v) {
    #if ENDIANESS == __ORDER_LITTLE_ENDIAN__
        _f_u_ t;
        t.f = v;
        t.u = htonl(t.u);
        return t.f;
    #else
        return v;
    #endif
}

/**
 * @brief Converts a float from network byte order to host byte order.
 * 
 * @param v The float value to convert.
 * 
 * @return The converted float value in host byte order.
 */
inline float ntohf(float v) {
    #if ENDIANESS == __ORDER_LITTLE_ENDIAN__
        _f_u_ t;
        t.f = v;
        t.u = ntohl(t.u);
        return t.f;
    #else
        return v;
    #endif
}

/**
 * @brief Converts a double from network byte order to host byte order.
 * 
 * @param v The double value to convert.
 * 
 * @return The converted double value in host byte order.
 */
inline double ntohd(double v) {
    #if ENDIANESS == __ORDER_LITTLE_ENDIAN__
        _d_u_ t;
        t.d = v;
        t.u = ntohl(t.u);
        return t.d;
    #else
        return v;
    #endif
}

/**
 * @brief Converts a double from host byte order to network byte order.
 * 
 * @param v The double value to convert.
 * 
 * @return The converted double value in network byte order.
 */
inline double htond(double v) {
    #if ENDIANESS == __ORDER_LITTLE_ENDIAN__
        _d_u_ t;
        t.d = v;
        t.u = htonl(t.u);
        return t.d;
    #else
        return v;
    #endif
}

/**
 * @brief Converts a decimal from host byte order to network byte order.
 * 
 * @param v The decimal value to convert.
 * 
 * @return The converted decimal value in network byte order.
 */
inline decimal htondec(decimal v) {
    #ifdef FOUND_FLOAT_MODE
        return htonf(v);
    #else
        return htond(v);
    #endif
}

/**
 * @brief Converts a decimal from network byte order to host byte order.
 * 
 * @param v The decimal value to convert.
 * 
 * @return The converted decimal value in host byte order.
 */
inline decimal ntohdec(decimal v) {
    #ifdef FOUND_FLOAT_MODE
        return htonf(v);
    #else
        return htond(v);
    #endif
}

/**
 * @brief Calculates the CRC32 checksum for a given data buffer.
 * 
 * @param data Pointer to the data buffer.
 * @param length The size of the data buffer in bytes.
 * 
 * @return The calculated CRC32 checksum.
 */
uint32_t calculateCRC32(const void* data, size_t length);

}  // namespace found

#endif  // SRC_DATAFILE_ENCODING_HPP_
