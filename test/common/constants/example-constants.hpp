/**
 * Include all relavant hpp files here where you need access to specific cpp files. The Makefile
 * is configured to use those files despite being in a seperate folder, so it will be recognized
*/
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "src/distance/edge.hpp"
#include "src/style/style.hpp"
#include "src/spatial/attitude-utils.hpp"

#include "test/common/mocks/example-mocks.hpp"

namespace found {
// Array that holds one
unsigned char arr[1] = {1};
// Expected result
Vec2 expectedPoint1 = {1.0, 2.0};
Vec2 expectedPoint2 = {3.0, 4.0};
Points expectedPoints = {expectedPoint1, expectedPoint2};
const long unsigned int expectedSize = 2;  // NOLINT
}  // namespace found
