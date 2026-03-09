#include <gtest/gtest.h>

#include "test/common/common.hpp"

#include "src/common/spatial/attitude-utils.hpp"
#include "src/common/decimal.hpp"

#define DOUBLE_DEFAULT_TOLERANCE 0.001
#define TLS_TOLERANCE 0.05

#define VECTOR3_EQUALS(vec1, vec2, tolerance) \
    EXPECT_LT(abs(vec1(0) - vec2(0)), tolerance); \
    EXPECT_LT(abs(vec1(1) - vec2(1)), tolerance); \
    EXPECT_LT(abs(vec1(2) - vec2(2)), tolerance);

#define VECTOR2_EQUALS(vec1, vec2, tolerance) \
    EXPECT_LT(abs(vec1(0) - vec2(0)), tolerance); \
    EXPECT_LT(abs(vec1(1) - vec2(1)), tolerance);

namespace found {

TEST(TLSTest, vec3Test) {
    Eigen::MatrixXd data {
        {1.0,1.0,1.0,3.0},
        {1.0,2.0,3.0,6.0},
        {0.0,0.0,0.0,0.0},
        {-1.0,-1.0,-1.0,-3.0},
        {-1.0,3.0,0.0,2.0},
        {4329.0, -4211.0, 0.0, 118.0}
    };
    Vec3 expected(1,1,1);
    Vec3 actual = TLS(data);
    VECTOR3_EQUALS(actual, expected, DOUBLE_DEFAULT_TOLERANCE);
}

TEST(TLSTest, vec2Test) {
    Eigen::MatrixXd data {
        {0.239794,0.0,0.719382},
        {1.0,2.0,5.0},
        {0.0,0.0,0.0},
        {-1.0,5.0,2.0}
    };
    Vec2 expected(3,1);
    Vec2 actual = TLS(data);
    VECTOR2_EQUALS(actual, expected, DOUBLE_DEFAULT_TOLERANCE);
}

TEST(TLSTest, vec2PermutatedTest) {
    Eigen::MatrixXd data {
        {0.239793,0.0,0.719383},
        {1.02,2.0,5.002},
        {0.001,0.0004,0.005},
        {-1.001,5.0,2.0}
    };
    Vec2 expected(3,1);
    Vec2 actual = TLS(data);
    VECTOR2_EQUALS(actual, expected, TLS_TOLERANCE);
}

}  // namespace found
