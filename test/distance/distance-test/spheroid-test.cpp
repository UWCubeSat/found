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

#define BALLPARK 5000 // we're within 5 kilometers; Department of War is probably fine with that right

// The equatorial radius of Earth (m)
#define RADIUS_OF_EARTH_A (static_cast<decimal>(6378.1366))
// The polar radius of Earth (m)
#define RADIUS_OF_EARTH_C (static_cast<decimal>(6356.7519))

#define VECTOR_EQUALS(vec1, vec2, tolerance) \
    EXPECT_LT(abs(vec1.x - vec2.x), tolerance); \
    EXPECT_LT(abs(vec1.y - vec2.y), tolerance); \
    EXPECT_LT(abs(vec1.z - vec2.z), tolerance);


//--position 10378137 0 0 --orientation 140 0 0 --focal-length 85e-3 --pixel-size 20e-6 --x-resolution 512 --y-resolution 512
TEST(SpheroidDistanceDeterminationAlgorithmTest, TestRandomEarth) {
    Vec3 principleAxisDimensions = {RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_C};
    int imageWidth = 700;
    int imageHeight = 600;
    Camera cam(0.05, .0000514, imageWidth, imageHeight); 
    Vec3 AOR = {0.366283, 0.69847598, -0.61479108};
    Points pts = {
        {static_cast<decimal>(220.52335207355273), static_cast<decimal>(517.0046392531248)},
        {static_cast<decimal>(52.64756721300976), static_cast<decimal>(593.7063270146842)},
        {static_cast<decimal>(676.361794698842), static_cast<decimal>(138.27797849246326)},
        {static_cast<decimal>(431.0872467696078), static_cast<decimal>(381.0657146828839)},
        {static_cast<decimal>(441.8347351393841), static_cast<decimal>(372.61653357409716)},
        {static_cast<decimal>(413.3147362648657), static_cast<decimal>(394.66834139040117)},
        {static_cast<decimal>(201.01053474171343), static_cast<decimal>(527.159698122975)},
        {static_cast<decimal>(442.78833932265997), static_cast<decimal>(371.88081375032795)},
        {static_cast<decimal>(92.41761180285141), static_cast<decimal>(577.5823435149246)},
        {static_cast<decimal>(224.33920692044197), static_cast<decimal>(514.976607424638)}
    };


    
    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), principleAxisDimensions, AOR);
    Vec3 expected = {3521.28856245,  4621.18068856, -5099.43664609};
    Vec3 actual = algo.Run(pts);
    
    std::stringstream ss1;
    LOG_INFO(ss1.str());

    VECTOR_EQUALS(expected, actual, BALLPARK);
}
