#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <cstdint>
#include <vector>

uint32_t computeChecksum(const std::vector<char>& data);

#endif // CHECKSUM_H