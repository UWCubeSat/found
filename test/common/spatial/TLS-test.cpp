#include <gtest/gtest.h>
#include <Eigen/Core>

#include "test/common/common.hpp"

#include "common/logging.hpp"

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


TEST(TLSTest, vec3Test) {
    Eigen::MatrixXd data {
        {1.0,1.0,1.0,3.0},
        {1.0,2.0,3.0,6.0},
        {0.0,0.0,0.0,0.0},
        {-1.0,-1.0,-1.0,-3.0},
        {-1.0,3.0,0.0,2.0},
        {4329.0, -4211.0, 0.0, 118.0}
    };
    Eigen::Vector3d expected(1,1,1);
    Eigen::Vector3d actual = found::TLS(data);
    VECTOR3_EQUALS(actual, expected, DOUBLE_DEFAULT_TOLERANCE);
}

TEST(TLSTest, vec2Test) {
    Eigen::MatrixXd data {
        {0.239794,0.0,0.719382},
        {1.0,2.0,5.0},
        {0.0,0.0,0.0},
        {-1.0,5.0,2.0}
    };
    Eigen::Vector2d expected(3,1);
    Eigen::Vector2d actual = found::TLS(data);
    VECTOR2_EQUALS(actual, expected, DOUBLE_DEFAULT_TOLERANCE);
}

TEST(TLSTest, vec2PermutatedTest) {
    Eigen::MatrixXd data {
        {0.239793,0.0,0.719383},
        {1.02,2.0,5.002},
        {0.001,0.0004,0.005},
        {-1.001,5.0,2.0}
    };
    Eigen::Vector2d expected(3,1);
    Eigen::Vector2d actual = found::TLS(data);
    VECTOR2_EQUALS(actual, expected, TLS_TOLERANCE);
}

// TEST(TLSTest, scalarInvalidTest) {
//     Eigen::Matrix<decimal,4,2> data {
//         {0.239794,0.719382},
//         {1,3},
//         {0,0},
//         {-1,-3}
//     };
//     ASSERT_ANY_THROW(found::TLS(data));
// }

// TEST(TLSTest, invalidSizeTest) {
//     Eigen::Matrix<decimal,2,1> data {
//         {1},
//         {1}
//     };
//     ASSERT_ANY_THROW(found::TLS(data));
// }
