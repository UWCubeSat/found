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
