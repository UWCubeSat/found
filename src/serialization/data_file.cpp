#include "serialization/data_file.hpp"
#include <stdexcept>

void DataFileHeader::validate() {
    if (magic[0] != 'F' || magic[1] != 'O' || magic[2] != 'U' || magic[3] != 'N') {
        throw std::runtime_error("Magic value mismatch");
    }

    if (version != 1) {
        throw std::runtime_error("Unsupported version");
    }
}
