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
    Vec3 result = lvga_1.Run(test_vector_1);
    ASSERT_TRUE(std::fabs(result.x + test_vector_1.x) <= 0.0001);
    ASSERT_TRUE(std::fabs(result.y + test_vector_1.y) <= 0.0001);
    ASSERT_TRUE(std::fabs(result.z + test_vector_1.z) <= 0.0001);
}

}  // namespace found
