#ifndef SRC_DATAFILE_DATAFILE_HPP_
#define SRC_DATAFILE_DATAFILE_HPP_

#include <memory>
#include <cstdint>
#include <string>

#include "common/spatial/attitude-utils.hpp"  // Includes Vec3 and EulerAngles
#include "common/style.hpp"

/**
 * @file datafile.hpp
 * @brief Declares data structures for serialized spatial data files, including headers,
 *        location records, and full data file representations.
 * 
 * @note All decimal variables are written as type double regardless of underlying
 * definition
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
    uint32_t version = 1U;

    /**
     * @brief Number of position entries (Vec3 elements) in the file.
     */
    uint32_t num_positions;

    /**
     * @brief CRC32 checksum of the header (excluding this field) for validation.
     */
    uint32_t crc;
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
    Quaternion relative_attitude;

    /**
     * @brief Collection of location records in the file.
     */
    std::unique_ptr<LocationRecord[]> positions;

    /**
     * The path of this DataFile.
     * 
     * @note This is not a saved field
     */
    std::string path;

    /// Constructs this
    DataFile() = default;

    /**
     * Moves another DataFile
     * 
     * @param other The file to copy
     */
    DataFile(DataFile &&other) noexcept = default;

    /**
     * Moves another DataFile
     * 
     * @param other The file to copy
     * 
     * @return The resulting data file (this)
     */
    DataFile &operator=(DataFile &&other) = default;
};

}  // namespace found

#endif  // SRC_DATAFILE_DATAFILE_HPP_
