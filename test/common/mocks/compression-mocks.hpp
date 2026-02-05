#ifndef TEST_COMMON_MOCKS_COMPRESSION_MOCKS_HPP_
#define TEST_COMMON_MOCKS_COMPRESSION_MOCKS_HPP_

#include <gmock/gmock.h>

#include "compression/compression.hpp"

namespace found {

class MockCompressionAlgorithm : public CompressionAlgorithm {
 public:
    MOCK_METHOD(CompressionResult, Run, (const std::string &imagePath), (override));
};

}  // namespace found

#endif  // TEST_COMMON_MOCKS_COMPRESSION_MOCKS_HPP_
