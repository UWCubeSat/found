#ifndef TEST_COMMON_COMMON_HPP_
#define TEST_COMMON_COMMON_HPP_

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <sstream>

#include "src/common/decimal.hpp"
#include "src/common/style.hpp"
#include "src/common/spatial/attitude-utils.hpp"
#include "src/common/spatial/camera.hpp"

namespace found {

///// Definition of custom ASSERT_* statements /////

#define DEFAULT_TOLERANCE DECIMAL(1e-3)

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
    if (val1.real < 0) val1 = -val1; \
    if (val2.real < 0) val2 = -val2; \
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

#define ASSERT_DF_EQ_DEFAULT(val1, val2) \
    ASSERT_EQ(val1.header.version, val2.header.version); \
    ASSERT_EQ(val1.header.num_positions, val2.header.num_positions); \
    /* ASSERT_EQ(val1.header.crc, val2.header.crc) */                       \
    ASSERT_QUAT_EQ_DEFAULT(val1.relative_attitude, val2.relative_attitude); \
    for (size_t i = 0; i < val1.header.num_positions; i++) { \
        ASSERT_VEC3_EQ_DEFAULT(val1.positions[i].position, val2.positions[i].position); \
        /* ASSERT_EQ(val1.positions[i].timestamp, val2.positions[i].timestamp); */ \
    }

#define ASSERT_DF_EQ(val1, val2, tolerance) \
    ASSERT_EQ(val1.header.version, val2.header.version); \
    ASSERT_EQ(val1.header.num_positions, val2.header.num_positions); \
    /* ASSERT_EQ(val1.header.crc, val2.header.crc) */                          \
    ASSERT_QUAT_EQ(val1.relative_attitude, val2.relative_attitude, tolerance); \
    for (size_t i = 0; i < val1.header.num_positions; i++) { \
        ASSERT_VEC3_EQ_DEFAULT(val1.positions[i].position, val2.positions[i].position); \
        /* ASSERT_EQ(val1.positions[i].timestamp, val2.positions[i].timestamp); */ \
    }

#define ASSERT_RANGE(num, lo, hi) \
    ASSERT_GE(num, lo) << "Value " << num << " is less than lower bound " << lo; \
    ASSERT_LE(num, hi) << "Value " << num << " is greater than upper bound " << hi;

///// Definition of Assets /////

/**
 * Represents an image with associated data
 */
struct ImageData {
    /// @brief The path of the image
    const char *path;

    /// @brief The image's focal length (m)
    decimal focal_length;
    std::string FocalLength;

    /// @brief The image's pixel size (m)
    decimal pixel_size;
    std::string PixelSize;

    /// @brief The image's orientation
    EulerAngles orientation;
    std::string Orientation;

    /// @brief The image's position (m)
    Vec3 position;
    std::string Position;

    /**
     * Initializes an ImageData struct
     * 
     * @param path The path of the image
     * @param focal_length The image focal length (m)
     * @param pixel_size The pixel size of the image (m)
     * @param orientation The orientation of the image
     * @param position The position of the image
     */
    ImageData(const char *path, decimal focal_length, decimal pixel_size, EulerAngles orientation, Vec3 position) :
            path(path), focal_length(focal_length), FocalLength(std::to_string(focal_length)), pixel_size(pixel_size),
            PixelSize(std::to_string(pixel_size)), orientation(orientation), position(position) {
        Orientation += orientation.ra;
        Orientation += ",";
        Orientation += orientation.de;
        Orientation += ",";
        Orientation += orientation.roll;

        Position += position.x;
        Position += ",";
        Position += position.y;
        Position += ",";
        Position += position.z;
    }
};

/// The temporary Datafile
constexpr const char *temp_df = "test/common/assets/temp.found";
/// The valid Position Data text
constexpr const char *pos_data = "test/common/assets/position-data/pos-data-valid.txt";
/// The sample image
const ImageData example_earth1(
    "test/common/assets/example_earth1.png",
    85e-3,
    20e-6,
    {DegToRad(140), 0, 0},
    {DECIMAL(10378137), 0, 0}
);

}  // namespace found

#endif  // TEST_COMMON_COMMON_HPP_
