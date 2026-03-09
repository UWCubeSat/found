#include <gtest/gtest.h>

#include <cmath>
#include <utility>
#include <string>

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
using found::DECIMAL

#define TOLERANCE 0.001 // testing analytical solutions; should be precise to floating point error

// The equatorial radius of Earth (m)
#define RADIUS_OF_EARTH_A (DECIMAL(6378.1366))
// The polar radius of Earth (m)
#define RADIUS_OF_EARTH_C (DECIMAL(6356.7519))


TEST(SpheroidDistanceDeterminationAlgorithmTest, TestRandomEarth1) {
    Vec3 principleAxisDimensions(RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_C);
    int imageWidth = 700;
    int imageHeight = 600;
    Camera cam(0.05, .0000514, imageWidth, imageHeight); 
    Quaternion attitude = SphericalToQuaternion(DegToRad(-36.3387), DegToRad(-151.885), DegToRad(279.474));
    Points pts = {
        {DECIMAL(611.7877295393952),  DECIMAL(264.09276117927675)},
        {DECIMAL(573.8922119671513),  DECIMAL(325.3968742337466)},
        {DECIMAL(350.4939091109065),  DECIMAL(589.3274589318719)},
        {DECIMAL(391.14346133188616), DECIMAL(550.233909535328)},
        {DECIMAL(464.0968679643527),  DECIMAL(471.5734768209798)},
        {DECIMAL(454.4893660268322),  DECIMAL(482.63970990423604)},
        {DECIMAL(658.5589230687222),  DECIMAL(177.48386735439033)},
        {DECIMAL(373.4109900481945),  DECIMAL(567.6612715009444)},
        {DECIMAL(684.6437823193647),  DECIMAL(122.43996936965047)},
        {DECIMAL(479.9742150253835),  DECIMAL(452.75848062170667)},
        {DECIMAL(518.2721763857571),  DECIMAL(404.41735985920894)},
        {DECIMAL(491.85561942267265), DECIMAL(438.2265392379015)},
        {DECIMAL(639.7657274333935),  DECIMAL(213.9521877859907)},
        {DECIMAL(694.1329604135699),  DECIMAL(100.94348878568302)},
        {DECIMAL(571.5349509252572),  DECIMAL(328.99120144536215)},
        {DECIMAL(379.6326629337276),  DECIMAL(561.6146678003332)},
        {DECIMAL(513.780424677385),   DECIMAL(410.31951218219683)},
        {DECIMAL(655.9153967653705),  DECIMAL(182.76347341186144)},
        {DECIMAL(340.53686956204075), DECIMAL(598.4563901280716)},
        {DECIMAL(392.5920815577219),  DECIMAL(548.783429938299)},
        {DECIMAL(443.72986548528297), DECIMAL(494.76101854907455)},
        {DECIMAL(470.3974489413311),  DECIMAL(464.18744060291704)},
        {DECIMAL(482.12392335127174), DECIMAL(450.15862072244306)},
    };
    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), principleAxisDimensions, attitude);
    Vec3 actual = algo.Run(pts);
    Vec3 expected = {4211.974212862954, 3307.209203472482, -5616.06183322837};

    Vec3 trueRp = {-5875.0, 4366.0, 2577.0};

    Vec3 computedRp = TPC.transpose() * actual;

    const Mat3 Ap = principleAxes_.cwiseInverse().asDiagonal();

    const Mat3 Ac = TPC.transpose() * Ap * TPC;

    decimal expectedDist = trueRp.norm();
    decimal actualDist = computedRp.norm();

    EXPECT_LT(abs(trueRp.norm() - computedRp.norm()), 0.0001);
    ASSERT_VEC2_EQ(actual, expected, TOLERANCE);
}
