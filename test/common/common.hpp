#ifndef COMMON_H
#define COMMON_H

#include <gtest/gtest.h>

#include "src/common/decimal.hpp"
#include "src/common/style.hpp"
#include "src/common/spatial/attitude-utils.hpp"

namespace found {

#define ASSERT_RANGE(num, lo, hi) \
    ASSERT_GE(num, lo) << "Value " << num << " is less than lower bound " << lo; \
    ASSERT_LE(num, hi) << "Value " << num << " is greater than upper bound " << hi;

#define DEFAULT_TOLERANCE 1e-3

constexpr auto vectorEqual = [](const Vec2 &a, const Vec2 &b) {
    return abs(a.x - b.x) < DEFAULT_TOLERANCE && abs(a.y - b.y) < DEFAULT_TOLERANCE;
};

constexpr auto Vec3Equal = [](const Vec3 &a, const Vec3 &b) {
    return abs(a.x - b.x) < DEFAULT_TOLERANCE && abs(a.y - b.y) < DEFAULT_TOLERANCE
           && abs(a.z - b.z) < DEFAULT_TOLERANCE;
};

constexpr auto LocationRecordEqual = [](const LocationRecord &a, const LocationRecord &b) {
    return a.timestamp == b.timestamp && Vec3Equal(a.position, b.position);
};

MATCHER_P(LocationRecordsEqual, expected, "") {
    return std::is_permutation(expected.begin(), expected.end(),
                                arg.begin(), arg.end(),
                                LocationRecordEqual);
}

#define ASSERT_DECIMAL_EQ(val1, val2, tolerance) ASSERT_LT(DECIMAL_ABS(val1 - val2), tolerance)

#define ASSERT_DECIMAL_EQ_DEFAULT(val1, val2) ASSERT_DECIMAL_EQ(val1, val2, DEFAULT_TOLERANCE)

#define ASSERT_VEC2_EQ(val1, val2, tolerance) \
    ASSERT_DECIMAL_EQ(val1.x, val2.x, tolerance); \
    ASSERT_DECIMAL_EQ(val1.y, val2.y, tolerance)

#define ASSERT_VEC2_EQ_DEFAULT(val1, val2) ASSERT_VEC2_EQ(val1, val2, DEFAULT_TOLERANCE)

#define ASSERT_VEC3_EQ(val1, val2, tolerance) \
    ASSERT_DECIMAL_EQ(val1.x, val2.x, tolerance); \
    ASSERT_DECIMAL_EQ(val1.y, val2.y, tolerance); \
    ASSERT_DECIMAL_EQ(val1.z, val2.z, tolerance)

#define ASSERT_VEC3_EQ_DEFAULT(val1, val2) ASSERT_VEC3_EQ(val1, val2, DEFAULT_TOLERANCE)

#define ASSERT_QUAT_EQ(val1, val2, tolerance) \
    ASSERT_DECIMAL_EQ(val1.real, val2.real, tolerance); \
    ASSERT_DECIMAL_EQ(val1.i, val2.i, tolerance); \
    ASSERT_DECIMAL_EQ(val1.j, val2.j, tolerance); \
    ASSERT_DECIMAL_EQ(val1.k, val2.k, tolerance)

#define ASSERT_QUAT_EQ_DEFAULT(val1, val2) ASSERT_QUAT_EQ(val1, val2, DEFAULT_TOLERANCE)

#define ASSERT_EA_EQ(val1, val2, tolerance) \
    { \
        Quaternion quat1 = SphericalToQuaternion(val1); \
        Quaternion quat2 = SphericalToQuaternion(val2); \
        if (quat1.real < 0) quat1 = -quat1; \
        if (quat2.real < 0) quat2 = -quat2; \
        ASSERT_QUAT_EQ(quat1, quat2, tolerance); \
    }

#define ASSERT_EA_EQ_DEFAULT(val1, val2) ASSERT_EA_EQ(val1, val2, DEFAULT_TOLERANCE)

#define ASSERT_IMAGE_EQ(val1, val2) \
    ASSERT_EQ(val1.width, val2.width); \
    ASSERT_EQ(val1.height, val2.height); \
    ASSERT_EQ(val1.channels, val2.channels); \
    for (int i = 0; i < val1.width * val1.height * val1.channels; ++i) { \
        ASSERT_EQ(val1.image[i], val2.image[i]); \
    }

}  // namespace found

#endif  // COMMON_H
