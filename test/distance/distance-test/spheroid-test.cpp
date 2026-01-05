#include <gtest/gtest.h>

#include <cmath>
#include <utility>

#include "common/style.hpp"
#include "common/spatial/attitude-utils.hpp"
#include "common/spatial/camera.hpp"
#include "distance/distance.hpp"
#include "common/logging.hpp"
#include "common/decimal.hpp"
#include "test/common/common.hpp"

using found::Camera;
using found::Vec3;
using found::Vec2;
using found::Mat3;
using found::Points;
using found::PositionVector;
using found::SpheroidDistanceDeterminationAlgorithm;

// The equatorial radius of Earth (m)
#define RADIUS_OF_EARTH_A (static_cast<decimal>(6378137.0))
// The polar radius of Earth (m)
#define RADIUS_OF_EARTH_C (static_cast<decimal>(6356752.3142))

#define VECTOR_EQUALS(vec1, vec2, tolerance) \
    EXPECT_LT(abs(vec1.x - vec2.x), tolerance); \
    EXPECT_LT(abs(vec1.y - vec2.y), tolerance); \
    EXPECT_LT(abs(vec1.z - vec2.z), tolerance);


Vec3 chooseCloserVec(Vec3 a, Vec3 b, Vec3 target){
    if (SquareDistance(a, target) < SquareDistance(b, target)) return a;
    return b;
}

//--position 10378137 0 0 --orientation 140 0 0 --focal-length 85e-3 --pixel-size 20e-6 --x-resolution 512 --y-resolution 512
TEST(SpheroidDistanceDeterminationAlgorithmTest, TestRandomEarth) {
    Vec3 principleAxisDimensions = {RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_C};
    int imageWidth = 512;
    int imageHeight = 512;
    Camera cam(0.085, 0.00002, imageWidth, imageHeight); 
    PositionVector expected = {298.3*10000, -16.4*10000, 790.45*10000};

    Mat3 conicSection = { 1, 3.27277381, 2.17897048,
              3.27277381,   1.40298645,   3.30539912,
              2.17897048,    3.30539912,  -1.44975746};


    
    SpheroidDistanceDeterminationAlgorithm algo = SpheroidDistanceDeterminationAlgorithm(std::move(cam), principleAxisDimensions, conicSection);

    Mat3 results = algo.TestRun();
    PositionVector actual = chooseCloserVec(results.Column(0), results.Column(1), expected);

    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}
