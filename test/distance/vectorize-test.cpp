#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "src/distance/vectorize.hpp"

#include "test/common/constants/vectorize-constants.hpp"

namespace found {

class VectorizeTest: public testing::Test {
 protected:
        void SetUp() override {}
        virtual void TearDown() {}
};

/**
 * Tests conversion when there is no attitude difference.
 * The result should match the input vector.
 */
TEST_F(VectorizeTest, IdentityTest) {
    Vec3 result = lvga1.Run(test_vector_1);
    ASSERT_TRUE(result.x == -test_vector_1.x);
    ASSERT_TRUE(result.y == -test_vector_1.y);
    ASSERT_TRUE(result.z == -test_vector_1.z);
}

}  // namespace found
