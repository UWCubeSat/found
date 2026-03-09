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

#define BALLPARK 0.001 // testing analytical solutions; should be precise to floating point error

// The equatorial radius of Earth (m)
#define RADIUS_OF_EARTH_A (static_cast<decimal>(6378.1366))
// The polar radius of Earth (m)
#define RADIUS_OF_EARTH_C (static_cast<decimal>(6356.7519))

#define VECTOR_EQUALS(vec1, vec2, tolerance) \
    EXPECT_LT(abs(vec1.x() - vec2.x()), tolerance); \
    EXPECT_LT(abs(vec1.y() - vec2.y()), tolerance); \
    EXPECT_LT(abs(vec1.z() - vec2.z()), tolerance);

// converting from Z up Y forward (world) to Y up Z forward (camera)
// Idk who's gonna do this in the actual algorithm
Mat3 ComputeBodyToCamTransformation(Vec3 attitude){
    decimal pi = 3.14159;
    Mat3 Xrot = {1, 0, 0,
        0, cos(attitude.x()), -sin(attitude.x()),
        0, sin(attitude.x()), cos(attitude.x())};
    // adjustment to get cam facing positive Y with X aligned to world
    Mat3 adjustment = {1, 0, 0,
        0, cos(pi/2), -sin(pi/2),
        0, sin(pi/2), cos(pi/2)};
    Mat3 Yrot = {cos(attitude.y()), 0, sin(attitude.y()),
        0, 1, 0,
        -sin(attitude.y()), 0, cos(attitude.y())};
    Mat3 Zrot = {cos(attitude.z()), -sin(attitude.z()), 0,
        sin(attitude.z()), cos(attitude.z()), 0,
        0, 0, 1};

    Mat3 invertZ = {1, 0, 0,
        0, 1, 0,
        0, 0, -1};

    Mat3 TCP = Zrot * Xrot * Yrot * adjustment * invertZ;
    return TCP.transpose();
}

void print(std::stringstream * ss, std::string str){
    *ss << str;
}
void bar(std::stringstream * ss){
    print(ss, "\n-----------------------------------------");
}
void print(std::stringstream * ss, std::string str, decimal num){
    *ss << "\n" << str << "\t:\t" << num;
}
void print(std::stringstream * ss, std::string str, Vec3 vec){
    *ss << "\n" << str << "\t:\t[" << vec.x() << ", "<< vec.y() << ", " << vec.z() << "]";
}

void conicEquation(std::stringstream * ss, Vec3 rc, Mat3 Ac, Mat3 calibrator){
    Mat3 C = (Ac * rc*rc.transpose() * (Ac)) + Ac*((rc.transpose() * Ac *rc-1)*-1);
    C = calibrator.transpose() * C * calibrator;
    
    C = C * (1/C(0,0));
    *ss << "\nConic Equation:\n" << C(0,0) << "x^2 + " << C(0,1) << "*2xy + " << C(1,1) << "*y^2 + " << C(0,2) <<"*2x + " << C(1,2) << "*2y+ " << C(2,2) << " = 0";
}



TEST(SpheroidDistanceDeterminationAlgorithmTest, TestRandomEarth1) {
    Vec3 principleAxisDimensions = {RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_C};
    int imageWidth = 700;
    int imageHeight = 600;
    Camera cam(0.05, .0000514, imageWidth, imageHeight); 
    Vec3 attitude = {-36.3387, -151.885, 279.474};
    Points pts = {{static_cast<decimal>(611.7877295393952), static_cast<decimal>(264.09276117927675)},{static_cast<decimal>(573.8922119671513), static_cast<decimal>(325.3968742337466)},{static_cast<decimal>(350.4939091109065), static_cast<decimal>(589.3274589318719)},{static_cast<decimal>(391.14346133188616), static_cast<decimal>(550.233909535328)},{static_cast<decimal>(464.0968679643527), static_cast<decimal>(471.5734768209798)},{static_cast<decimal>(454.4893660268322), static_cast<decimal>(482.63970990423604)},{static_cast<decimal>(658.5589230687222), static_cast<decimal>(177.48386735439033)},{static_cast<decimal>(373.4109900481945), static_cast<decimal>(567.6612715009444)},{static_cast<decimal>(684.6437823193647), static_cast<decimal>(122.43996936965047)},{static_cast<decimal>(479.9742150253835), static_cast<decimal>(452.75848062170667)},{static_cast<decimal>(518.2721763857571), static_cast<decimal>(404.41735985920894)},{static_cast<decimal>(491.85561942267265), static_cast<decimal>(438.2265392379015)},{static_cast<decimal>(639.7657274333935), static_cast<decimal>(213.9521877859907)},{static_cast<decimal>(694.1329604135699), static_cast<decimal>(100.94348878568302)},{static_cast<decimal>(571.5349509252572), static_cast<decimal>(328.99120144536215)},{static_cast<decimal>(379.6326629337276), static_cast<decimal>(561.6146678003332)},{static_cast<decimal>(513.780424677385), static_cast<decimal>(410.31951218219683)},{static_cast<decimal>(655.9153967653705), static_cast<decimal>(182.76347341186144)},{static_cast<decimal>(340.53686956204075), static_cast<decimal>(598.4563901280716)},{static_cast<decimal>(392.5920815577219), static_cast<decimal>(548.783429938299)},{static_cast<decimal>(443.72986548528297), static_cast<decimal>(494.76101854907455)},{static_cast<decimal>(470.3974489413311), static_cast<decimal>(464.18744060291704)},{static_cast<decimal>(482.12392335127174), static_cast<decimal>(450.15862072244306)},
};
    Mat3 TPC = ComputeBodyToCamTransformation(attitude).transpose();
    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), principleAxisDimensions, TPC);
    Vec3 actual = algo.Run(pts);
    Vec3 expected = {4211.974212862954, 3307.209203472482, -5616.06183322837};

    Vec3 trueRp = {-5875.0, 4366.0, 2577.0};

    Vec3 computedRp = TPC.transpose() * actual;

    const Mat3 Ap = {1/(RADIUS_OF_EARTH_A*RADIUS_OF_EARTH_A),0,0,
                              0,1/(RADIUS_OF_EARTH_A*RADIUS_OF_EARTH_A),0,
                              0,0,1/(RADIUS_OF_EARTH_C*RADIUS_OF_EARTH_C)};

    const Mat3 Ac = TPC.transpose() * Ap * TPC;

    decimal expectedDist = trueRp.norm();
    decimal actualDist = computedRp.norm();

    std::stringstream ss1;
    print(&ss1,"\n");
    conicEquation(&ss1, actual, Ac, cam.GetInverseCalibrationMatrix());
    print(&ss1, "True  rp", trueRp);
    print(&ss1, "True |rp|", expectedDist);
    bar(&ss1);
    print(&ss1, "Calc  rp", computedRp);
    print(&ss1, "Calc |rp|", actualDist);
    bar(&ss1);
    print(&ss1, "Diff", abs(expectedDist-actualDist));
    print(&ss1, "Error", (expectedDist-actualDist)/expectedDist);
    print(&ss1,"\n");
    LOG_INFO(ss1.str());
    EXPECT_LT(abs(trueRp.norm() - computedRp.norm()), 0.0001);
    VECTOR_EQUALS(actual, expected, BALLPARK);
}
