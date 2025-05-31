#include "serialization/data_file.hpp"
#include <stdexcept>

namespace found {

/**
 * @brief Validates the integrity of the DataFileHeader.
 * 
 * This function checks the following conditions:
 * - The `magic` field must contain the characters 'F', 'O', 'U', 'N' in order.
 *   If this condition is not met, a `std::runtime_error` is thrown with the
 *   message "Magic value mismatch".
 * - The `version` field must be equal to 1. If the version is unsupported,
 *   a `std::runtime_error` is thrown with the message "Unsupported version".
 * 
 * @throws std::runtime_error If the `magic` value does not match the expected
 *         sequence or if the `version` is unsupported.
 */
void DataFileHeader::validate() {
    if (magic[0] != 'F' || magic[1] != 'O' || magic[2] != 'U' || magic[3] != 'N') {
        throw std::runtime_error("Magic value mismatch");
    }
    if (version != 1) {
        throw std::runtime_error("Unsupported version");
    }
}

}  // namespace found
