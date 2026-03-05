#include <gtest/gtest.h>
#include <Eigen/Core>

#include "common/logging.hpp"
#include "test/common/common.hpp"

#include "src/common/spatial/attitude-utils.hpp"
#include "src/common/decimal.hpp"

#define TLS_TOLERANCE decimal(0.05)

#define VECTOR3_EQUALS(vec1, vec2, tolerance) \
    EXPECT_LT(abs(vec1.x() - vec2.x()), tolerance); \
    EXPECT_LT(abs(vec1.y() - vec2.y()), tolerance); \
    EXPECT_LT(abs(vec1.z() - vec2.z()), tolerance);

#define VECTOR2_EQUALS(vec1, vec2, tolerance) \
    EXPECT_LT(abs(vec1.x() - vec2.x()), tolerance); \
    EXPECT_LT(abs(vec1.y() - vec2.y()), tolerance);


TEST(TLSTest, vec3Test) {
    Eigen::Matrix<decimal,6,4> data {
        {1,1,1,3},
        {1,2,3,6},
        {0,0,0,0},
        {-1,-1,-1,-3},
        {-1,3,0,2},
        {4329, -4211, 0, 118}
    };
    Eigen::Vector3d expected(1,1,1);
    Eigen::Vector3d actual = found::TLS(data);
    VECTOR3_EQUALS(actual, expected, DEFAULT_TOLERANCE);
}

TEST(TLSTest, vec2Test) {
    Eigen::Matrix<decimal,4,3> data {
        {0.239794,0,0.719382},
        {1,2,5},
        {0,0,0},
        {-1,5,2}
    };
    Eigen::Vector<decimal, 2> expected(3,1);
    Eigen::Vector<decimal, 2> actual = found::TLS(data);
    VECTOR2_EQUALS(actual, expected, DEFAULT_TOLERANCE);
}

TEST(TLSTest, vec2PermutatedTest) {
    Eigen::Matrix<decimal,4,3> data {
        {0.239793,0,0.719383},
        {1.02,2,5.002},
        {0.001,0.0004,0.005},
        {-1.001,5,2}
    };
    Eigen::Vector<decimal, 2> expected(3,1);
    Eigen::Vector<decimal, 2> actual = found::TLS(data);
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
