#include <fstream>
#include <iostream>
#include "serialization/encoding.hpp"
#include "serialization/serialization.hpp"
#include "common/spatial/attitude-utils.hpp"


namespace found {

/// Converts a DataFileHeader from host to network byte order.
void hton(DataFileHeader& header) {
    header.version = htonl(header.version);
    header.num_positions = htonl(header.num_positions);
    header.crc = htonl(header.crc);
}

/// Converts EulerAngles from host to network byte order.
void hton(EulerAngles& angles) {
    angles.roll = htondec(angles.roll);
    angles.ra = htondec(angles.ra);
    angles.de = htondec(angles.de);
}

/// Converts a Vec3 from host to network byte order.
void hton(Vec3& v) {
    v.x = htondec(v.x);
    v.y = htondec(v.y);
    v.z = htondec(v.z);
}

/// Converts a DataFileHeader from network to host byte order.
void ntoh(DataFileHeader& header) {
    header.version = ntohl(header.version);
    header.num_positions = ntohl(header.num_positions);
    header.crc = ntohl(header.crc);
}

/// Converts EulerAngles from network to host byte order.
void ntoh(EulerAngles& angles) {
    angles.roll = ntohdec(angles.roll);
    angles.ra = ntohdec(angles.ra);
    angles.de = ntohdec(angles.de);
}

/// Converts a Vec3 from network to host byte order.
void ntoh(Vec3& v) {
    v.x = ntohdec(v.x);
    v.y = ntohdec(v.y);
    v.z = ntohdec(v.z);
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
void serialize(const DataFile& data, std::ostream& stream) {
    DataFileHeader header = data.header;
    hton(header);
    header.crc = calculateCRC32(&header, sizeof(header) - sizeof(header.crc));
    header.crc = htonl(header.crc);
    stream.write(reinterpret_cast<const char*>(&header), sizeof(header));

    EulerAngles relative_attitude = data.relative_attitude;
    hton(relative_attitude);
    stream.write(reinterpret_cast<const char*>(&relative_attitude), sizeof(relative_attitude));

    for (auto& r : data.positions) {
        LocationRecord record = r;
        hton(record.position);
        record.timestamp = htonl(record.timestamp);
        stream.write(reinterpret_cast<const char*>(&record), sizeof(LocationRecord));
    }
}

/// Deserializes a DataFile object from an input stream.
DataFile deserialize(std::istream& stream) {
    DataFile data;
    data.header = readHeader(stream);

    stream.read(reinterpret_cast<char*>(&data.relative_attitude), sizeof(EulerAngles));
    if (stream.gcount() != sizeof(EulerAngles)) {
        throw std::ios_base::failure("Failed to read relative_attitude");
    }
    ntoh(data.relative_attitude);

    data.positions.reserve(data.header.num_positions);
    for (uint32_t i = 0; i < data.header.num_positions; ++i) {
        LocationRecord record;
        stream.read(reinterpret_cast<char*>(&record), sizeof(LocationRecord));
        if (stream.gcount() != sizeof(LocationRecord)) {
            throw std::ios_base::failure("Failed to read location record");
        }
        ntoh(record.position);
        record.timestamp = ntohl(record.timestamp);
        data.positions.push_back(record);
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

    // Validate CRC
    uint32_t expected_crc = calculateCRC32(&header, sizeof(header) - sizeof(header.crc));
    if (ntohl(header.crc) != expected_crc) {
        std::cerr << "Expected CRC: " << expected_crc << ", Found CRC: " << ntohl(header.crc) << std::endl;
        throw std::runtime_error("Header CRC validation failed: Corrupted file");
    }

    // Convert version and other fields from network to host byte order
    ntoh(header);

    return header;
}

}  // namespace found
