#include <fstream>
#include <iostream>
#include "serialization/serialization.hpp"
#include "spatial/attitude-utils.hpp"

using Vec3 = found::Vec3;

namespace found {

    void hton(DataFileHeader& header) {
        header.version = htonl(header.version);
        header.num_positions = htonl(header.num_positions);
        header.crc = htonl(header.crc);
    }

    void hton(EulerAngles& angles) {
        angles.roll = htonl(angles.roll);
        angles.ra = htonl(angles.ra);
        angles.de = htonl(angles.de);
    }

    void hton(Vec3& v) {
        v.x = htonl(v.x);
        v.y = htonl(v.y);
        v.z = htonl(v.z);
    }

    void ntoh(DataFileHeader& header) {
        header.version = ntohl(header.version);
        header.num_positions = ntohl(header.num_positions);
        header.crc = ntohl(header.crc);
    }

    void ntoh(EulerAngles& angles) {
        angles.roll = ntohl(angles.roll);
        angles.ra = ntohl(angles.ra);
        angles.de = ntohl(angles.de);
    }

    void ntoh(Vec3& v) {
        v.x = ntohl(v.x);
        v.y = ntohl(v.y);
        v.z = ntohl(v.z);
    }

    uint32_t calculateCRC32(const void* data, size_t length) {
        uint32_t crc = 0;
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        for (size_t i = 0; i < length; ++i) {
            crc += bytes[i];
        }
        return crc;
    }

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

    DataFile deserialize(std::istream& stream) {
        DataFile data;

        data.relative_attitude = EulerAngles(0, 0, 0);
        ntoh(data.relative_attitude);

        data.header = readHeader(stream);

        stream.read(reinterpret_cast<char*>(&data.relative_attitude), sizeof(data.relative_attitude));

        for (auto& r : data.positions) {
            stream.read(reinterpret_cast<char*>(&r), sizeof(LocationRecord));
            ntoh(r.position);
            r.timestamp = ntohl(r.timestamp);
        }

        data.positions.resize(data.header.num_positions);
        stream.read(reinterpret_cast<char*>(data.positions.data()), data.positions.size() * sizeof(LocationRecord));

        return data;
    }

    DataFileHeader readHeader(std::istream& stream) {
        DataFileHeader header;
        stream.read(reinterpret_cast<char*>(&header), sizeof(header));

        uint32_t expected_crc = calculateCRC32(&header, sizeof(header) - sizeof(header.crc));
        if (ntohl(header.crc) != expected_crc) {
            throw std::runtime_error("Header CRC validation failed: Corrupted file");
        }

        ntoh(header);

        return header;
    }

}  // namespace found
