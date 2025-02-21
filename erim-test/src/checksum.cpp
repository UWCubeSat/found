#include "checksum.hpp"

uint32_t computeChecksum(const std::vector<char>& data) {
    uint32_t checksum = 0;
    for (char byte : data) {
        checksum += static_cast<uint8_t>(byte);
    }
    return checksum;
}