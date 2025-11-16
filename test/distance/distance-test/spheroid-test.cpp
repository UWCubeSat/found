#include <gtest/gtest.h>

#include <cmath>
#include <utility>

#include "common/style.hpp"
#include "common/spatial/attitude-utils.hpp"
#include "common/spatial/camera.hpp"
#include "distance/distance.hpp"
#include "common/logging.hpp"
#include "common/decimal.hpp"

using found::Camera;
using found::Vec3;
using found::Mat3;
using found::Points;
using found::PositionVector;
using found::SpheroidDistanceDeterminationAlgorithm;

// The equatorial radius of Earth (m)
#define RADIUS_OF_EARTH_A (static_cast<decimal>(6378137.0))
// The polar radius of Earth (m)
#define RADIUS_OF_EARTH_C (static_cast<decimal>(6356752.3142))

/* Test Macros */

/**
 * Requires that vec1 == vec2 (using DecimalEquals)
 * 
 * @param vec1 A Vec3 object
 * @param vec2 A Vec3 object
 * @param tolerance The tolerance for vec1 to be
 * "equal" to vec2
 * 
 * @post Will have REQUIRE'd that vec1 is equal to
 * vec2, on a component basis, within tolerance
*/
#define VECTOR_EQUALS(vec1, vec2, tolerance) \
    EXPECT_LT(abs(vec1.x - vec2.x), tolerance); \
    EXPECT_LT(abs(vec1.y - vec2.y), tolerance); \
    EXPECT_LT(abs(vec1.z - vec2.z), tolerance);

std::ostream &operator<<(std::ostream &stream, const Vec3 &vector) {
    stream << std::fixed << std::setprecision(5) << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";
    return stream;
}
