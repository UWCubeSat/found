/**
 * This is our constants file. We define it as an .hpp file simply
 * because all our variables are "constants". In practice, you should
 * aim to define ANY variable or function that you tend to use
 * repeatedly in a constants file
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
unsigned char imageContents[1] = {1};
Image image = {imageContents, {1, 0, 1}};

/**
 * Expected Results from our Mock
 */

// Expected Points
Vec2 expectedPoint1 = {1.0, 2.0};
Vec2 expectedPoint2 = {3.0, 4.0};
Points expectedPoints = {expectedPoint1, expectedPoint2};
// Expected Size of Points
const long unsigned int expectedSize = 2;  // NOLINT
}  // namespace found
