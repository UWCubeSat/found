#ifndef DATA_FILE_H
#define DATA_FILE_H

#include <vector>
#include <cstdint>
#include <string>
#include "spatial/attitude-utils.hpp"  // Include the header for Vec3

/**
 * @file data_file.hpp
 * @brief Defines structures for handling serialized data files, including headers, 
 *        location records, and complete data file representations.
 *
 * This file provides:
 * - `DataFileHeader`: Metadata for serialized data files, including validation methods.
 * - `LocationRecord`: A single location with position and timestamp.
 * - `DataFile`: A complete file containing header, attitude, and position records.
 */

namespace found {

    /**
     * @brief Represents the header of a data file used in the serialization process.
     *
     * Contains metadata such as a magic identifier, version, number of positions,
     * and a CRC value for validating header integrity.
     */
    struct DataFileHeader {
        /**
         * @brief A 4-character array used as a magic identifier for the file. Default: {'F', 'O', 'U', 'N'}.
         */
        char magic[4] = {'F', 'O', 'U', 'N'};

        /**
         * @brief The version of the data file format. Default: 1.
         */
        uint32_t version = 1;

        /**
         * @brief Number of Vec3 elements (positions) in the data file.
         */
        uint32_t num_positions;

        /**
         * @brief CRC32 checksum used to validate the integrity of the header.
         */
        uint32_t crc;

        /**
         * @brief Validates the integrity of the header using the magic bytes and CRC.
         */
        void validate();
    };

    /**
     * @brief Represents a single location record containing a position and a timestamp.
     */
    struct LocationRecord {
        /**
         * @brief The 3D position vector associated with this location.
         */
        Vec3 position;

        /**
         * @brief The timestamp corresponding to this location (in microseconds or appropriate unit).
         */
        uint64_t timestamp;
    };

    /**
     * @brief Represents a complete data file containing header, relative attitude, and position records.
     */
    struct DataFile {
        /**
         * @brief The metadata header of the file.
         */
        DataFileHeader header;

        /**
         * @brief Attitude of the object represented by Euler angles.
         */
        EulerAngles relative_attitude = EulerAngles(0, 0, 0);

        /**
         * @brief A list of recorded positions with timestamps.
         */
        std::vector<LocationRecord> positions;
    };

}  // namespace found

#endif  // DATA_FILE_H
