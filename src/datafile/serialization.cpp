#include <memory>
#include <fstream>
#include <iostream>
#include "datafile/encoding.hpp"
#include "datafile/serialization.hpp"
#include "common/spatial/attitude-utils.hpp"
#include "common/logging.hpp"

namespace found {

/// Converts a DataFileHeader from host to network byte order.
void hton(DataFileHeader& header) {
    header.version = htonl(header.version);
    header.num_positions = htonl(header.num_positions);
    header.crc = htonl(header.crc);
}

/// Converts a DataFileHeader from network to host byte order.
void ntoh(DataFileHeader& header) {
    header.version = ntohl(header.version);
    header.num_positions = ntohl(header.num_positions);
    header.crc = ntohl(header.crc);
}

/**
 *
 * @brief Reads a decimal value from the given input stream.
 *
 */
inline void read(std::istream& stream, decimal& value) {
    stream.read(reinterpret_cast<char*>(&value), sizeof(decimal));
    if (stream.gcount() != sizeof(decimal)) {
        throw std::ios_base::failure("Failed to read decimal value");
    }
    value = ntohdec(value);
}

/**
 *
 * @brief Writes a decimal value to the given output stream in network byte order.
 *
 */
inline void write(std::ostream& stream, const decimal& value) {
    decimal v = htondec(value);
    stream.write(reinterpret_cast<const char*>(&v), sizeof(decimal));
}

/**
 *
 * @brief Writes a 64-bit unsigned integer to the given output stream in network byte order.
 *
 */
inline void write(std::ostream& stream, const uint64_t& value) {
    uint64_t v = htonl(value);
    stream.write(reinterpret_cast<const char*>(&v), sizeof(uint64_t));
}

/**
 *
 * @brief Reads a 64-bit unsigned integer from the given input stream.
 *
 */
inline void read(std::istream& stream, uint64_t& value) {
    stream.read(reinterpret_cast<char*>(&value), sizeof(uint64_t));
    if (stream.gcount() != sizeof(uint64_t)) {  // GCOVR_EXCL_LINE (impossible to be true at this time)
        throw std::ios_base::failure("Failed to read uint64_t value");
    }
    value = ntohl(value);
}

/**
 *
 * @brief Reads a 32-bit unsigned integer from the given input stream in network byte order.
 *
 */
inline void read(std::istream& stream, uint32_t& value) {
    stream.read(reinterpret_cast<char*>(&value), sizeof(uint32_t));
    if (stream.gcount() != sizeof(uint32_t)) {
        throw std::ios_base::failure("Failed to read uint32_t value");
    }
    value = ntohl(value);
}

/**
 *
 * @brief Writes a 32-bit unsigned integer to the given output stream in network byte order.
 *
 */
inline void write(std::ostream& stream, const uint32_t& value) {
    uint32_t v = htonl(value);
    stream.write(reinterpret_cast<const char*>(&v), sizeof(uint32_t));
}

/**
 *
 * @brief Serializes an EulerAngles object to the given output stream.
 *
 */
inline void write(std::ostream& stream, const EulerAngles& angles) {
    write(stream, angles.roll);
    write(stream, angles.ra);
    write(stream, angles.de);
}

/**
 *
 * @brief Reads EulerAngles data from an input stream.
 *
 */
inline void read(std::istream& stream, EulerAngles& angles) {
    read(stream, angles.roll);
    read(stream, angles.ra);
    read(stream, angles.de);
}

/**
 *
 * @brief Serializes a Vec3 object to the given output stream.
 *
 */
inline void write(std::ostream& stream, const Vec3& v) {
    write(stream, v.x);
    write(stream, v.y);
    write(stream, v.z);
}

/**
 *
 * @brief Serializes a LocationRecord object to the given output stream.
 *
 */
inline void write(std::ostream& stream, const LocationRecord& record) {
    write(stream, record.position);
    write(stream, record.timestamp);
}

/**
 *
 * @brief Reads a Vec3 object from the given input stream.
 *
 */
inline void read(std::istream& stream, Vec3& v) {
    read(stream, v.x);
    read(stream, v.y);
    read(stream, v.z);
}

/**
 *
 * @brief Reads data from the input stream into a LocationRecord object.
 *
 */
inline void read(std::istream& stream, LocationRecord& record) {
    read(stream, record.position);
    read(stream, record.timestamp);
}

/// Calculates the CRC32 checksum for a block of memory.
uint32_t calculateCRC32(const void* data, size_t length) {
    uint32_t crc = 0xFFFFFFFFU;
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    for (size_t i = 0; i < length; ++i) {
        crc ^= bytes[i];
        for (int j = 0; j < 8; ++j) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320U;
            else
                crc = crc >> 1;
        }
    }
    return crc ^ 0xFFFFFFFFU;
}

/// Serializes a DataFile object to an output stream.
/// The number of positions in header must match the number of positions in entry.
void serialize(const DataFile& data, std::ostream& stream) {
    DataFileHeader header = data.header;
    header.crc = calculateCRC32(&header, sizeof(header) - sizeof(header.crc));
    hton(header);
    stream.write(reinterpret_cast<const char*>(&header), sizeof(header));

    write(stream, data.relative_attitude);

    for (uint32_t i = 0; i < data.header.num_positions; ++i) {
        write(stream, data.positions[i]);
    }
}

/// Deserializes a DataFile object from an input stream.
DataFile deserialize(std::istream& stream) {
    DataFile data;
    data.header = readHeader(stream);

    read(stream, data.relative_attitude);

    data.positions = std::make_unique<LocationRecord[]>(data.header.num_positions);
    for (uint32_t i = 0; i < data.header.num_positions; ++i) {
        read(stream, data.positions[i]);
    }

    return data;
}


/// Validates the magic number in the header.
bool isValidMagicNumber(const char magic[4]) {
    return magic[0] == 'F' && magic[1] == 'O' && magic[2] == 'U' && magic[3] == 'N';
}

DataFileHeader readHeader(std::istream& stream) {
    DataFileHeader header;
    stream.read(reinterpret_cast<char*>(&header), sizeof(DataFileHeader));
    if (stream.gcount() != sizeof(header)) {
        throw std::ios_base::failure("Failed to read header");
    }
    if (!isValidMagicNumber(header.magic)) {
        throw std::ios_base::failure("Invalid magic number in header");
    }

    // Convert version and other fields from network to host byte order
    ntoh(header);

    // Validate CRC
    uint32_t expected_crc = calculateCRC32(&header, sizeof(header) - sizeof(header.crc));
    if (header.crc != expected_crc) {
        LOG_ERROR("Expected CRC: " << expected_crc << ", Found CRC: " << ntohl(header.crc));
        throw std::ios_base::failure("Header CRC validation failed: Corrupted file");
    }

    return header;
}

}  // namespace found
