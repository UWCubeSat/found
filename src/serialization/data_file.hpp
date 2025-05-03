#ifndef DATA_FILE_H
#define DATA_FILE_H

#include <vector>
#include <cstdint>
#include <string>
#include "spatial/attitude-utils.hpp"  // Includes Vec3 and EulerAngles

/**
 * @file data_file.hpp
 * @brief Declares data structures for serialized spatial data files, including headers,
 *        location records, and full data file representations.
 */

namespace found {

    /**
     * @brief Represents the header of a data file used in the serialization process.
     *
     * Contains metadata such as a magic identifier, version, number of position records,
     * and a CRC32 checksum for integrity verification.
     */
    struct DataFileHeader {
        /**
         * @brief Magic identifier used to validate file type.
         *
         * The magic string "FOUN" is used to identify this file format during deserialization.
         * This is similar to other file formats (e.g., PNG, PDF) that include a recognizable
         * signature at the start of the file. If the magic value doesn't match, the file
         * is considered invalid or corrupted.
         */
        char magic[4] = {'F', 'O', 'U', 'N'};

        /**
         * @brief Version of the file format.
         * Default is 1. Used to support versioned deserialization in the future.
         */
        uint32_t version = 1;

        /**
         * @brief Number of position entries (Vec3 elements) in the file.
         */
        uint32_t num_positions;

        /**
         * @brief CRC32 checksum of the header (excluding this field) for validation.
         */
        uint32_t crc;

        /**
         * @brief Validates the integrity of the header using the magic string and CRC.
         *
         * Throws a runtime error if validation fails.
         */
        void validate();
    };

    /**
     * @brief Represents a single spatial data point with position and timestamp.
     */
    struct LocationRecord {
        /**
         * @brief 3D position of the recorded data point.
         */
        Vec3 position;

        /**
         * @brief Timestamp associated with the position, in microseconds or appropriate units.
         */
        uint64_t timestamp;
    };

    /**
     * @brief Represents a complete serialized data file.
     *
     * Contains the file header, relative orientation of the sensor, and a list of spatial position records.
     */
    struct DataFile {
        /**
         * @brief Metadata header for the file (includes magic, version, and CRC).
         */
        DataFileHeader header;

        /**
         * @brief Relative orientation (attitude) of the object as Euler angles.
         */
        EulerAngles relative_attitude = EulerAngles(0, 0, 0);

        /**
         * @brief Collection of location records in the file.
         */
        std::vector<LocationRecord> positions;
    };

}  // namespace found

#endif  // DATA_FILE_H
