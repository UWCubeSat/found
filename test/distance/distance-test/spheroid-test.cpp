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
#include "test/distance/distance-test/testData.hpp"

using found::Camera;
using found::Vec3;
using found::Vec2;
using found::Mat3;
using found::Points;
using found::PositionVector;
using found::SpheroidDistanceDeterminationAlgorithm;
using found::bigAssImage;

#define BALLPARK 0.001 // testing analytical solutions; should be precise to floating point error

// The equatorial radius of Earth (m)
#define RADIUS_OF_EARTH_A (static_cast<decimal>(6378.1366))
// The polar radius of Earth (m)
#define RADIUS_OF_EARTH_C (static_cast<decimal>(6356.7519))

#define VECTOR_EQUALS(vec1, vec2, tolerance) \
    EXPECT_LT(abs(vec1.x - vec2.x), tolerance); \
    EXPECT_LT(abs(vec1.y - vec2.y), tolerance); \
    EXPECT_LT(abs(vec1.z - vec2.z), tolerance);



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
    *ss << "\n" << str << "\t:\t[" << vec.x << ", "<< vec.y << ", " << vec.z << "]";
}

void conicEquation(std::stringstream * ss, Vec3 rc, Mat3 Ac){
    Mat3 C = (Ac * rc.OuterProduct(rc) * (Ac)) + Ac*((rc.OuterProduct(Ac)*rc-1)*-1);
    C = C * (1/C.At(0,0));
    *ss << "\nConic Equation:\n" << C.At(0,0) << "x^2 + " << C.At(0,1) << "*2xy + " << C.At(1,1) << "*y^2 + " << C.At(0,2) <<"*2x + " << C.At(1,2) << "*2y+ " << C.At(2,2) << " = 0";
}



TEST(SpheroidDistanceDeterminationAlgorithmTest, TestRandomEarth1) {
    Vec3 principleAxisDimensions = {RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_C};
    int imageWidth = 700;
    int imageHeight = 600;
    Camera cam(0.05, .0000514, imageWidth, imageHeight); 
    Vec3 AOR = {-0.16754666,  0.28948027, -0.94240612};
    Points pts = {{static_cast<decimal>(677.5358753014197), static_cast<decimal>(510.15044202539)},{static_cast<decimal>(645.4936938723756), static_cast<decimal>(537.8532693624234)},{static_cast<decimal>(666.8925017023865), static_cast<decimal>(519.512917282545)},{static_cast<decimal>(571.6925155472582), static_cast<decimal>(596.4595861829636)},{static_cast<decimal>(650.229882251296), static_cast<decimal>(533.8486928114402)},{static_cast<decimal>(653.4711543060477), static_cast<decimal>(531.0903381321456)},{static_cast<decimal>(583.2444742484045), static_cast<decimal>(587.7384580802305)},{static_cast<decimal>(624.0162819052008), static_cast<decimal>(555.6329035292347)},{static_cast<decimal>(690.0881275883015), static_cast<decimal>(498.89791667430774)},{static_cast<decimal>(595.7197721001701), static_cast<decimal>(578.1371751716015)},{static_cast<decimal>(654.2496467562646), static_cast<decimal>(530.425671137898)},{static_cast<decimal>(603.9238800113577), static_cast<decimal>(571.7175135288273)},{static_cast<decimal>(627.2821922796454), static_cast<decimal>(552.9688895751124)},{static_cast<decimal>(647.3614673111051), static_cast<decimal>(536.2776843167289)},{static_cast<decimal>(674.5449888460973), static_cast<decimal>(512.7978029753647)},{static_cast<decimal>(692.2624233665705), static_cast<decimal>(496.92519609493945)},{static_cast<decimal>(686.1580981320584), static_cast<decimal>(502.44585062483765)},{static_cast<decimal>(584.422851297312), static_cast<decimal>(586.8397423024081)},{static_cast<decimal>(610.9917589996783), static_cast<decimal>(566.1186642910959)},{static_cast<decimal>(594.6884000765087), static_cast<decimal>(578.9382493445212)},{static_cast<decimal>(577.064420433236), static_cast<decimal>(592.4241359293442)},{static_cast<decimal>(623.7793506281705), static_cast<decimal>(555.8256237233293)},{static_cast<decimal>(682.48407597481), static_cast<decimal>(505.74212401109503)},
};
    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), principleAxisDimensions, AOR);
    Vec3 actual = algo.Run(pts);
    Vec3 expected = {2758.45431576228, 3689.6447536370315, -5824.432376674428};

    Vec3 trueRp = {-4214.133704672528, -489.8481180046763, 6094.8902427420635};

    Mat3 computedTCP = algo.ComputeBodyToCamTransformation(AOR).Transpose();
    Vec3 computedRp = computedTCP * actual;

    const Mat3 Ap = {1/(RADIUS_OF_EARTH_A*RADIUS_OF_EARTH_A),0,0,
                              0,1/(RADIUS_OF_EARTH_A*RADIUS_OF_EARTH_A),0,
                              0,0,1/(RADIUS_OF_EARTH_C*RADIUS_OF_EARTH_C)};

    const Mat3 Ac = computedTCP * Ap * computedTCP.Transpose();

    std::stringstream ss1;
    print(&ss1,"\n");
    conicEquation(&ss1, actual, Ac);
    print(&ss1, "TCP Det  ", computedTCP.Det());
    bar(&ss1);
    print(&ss1, "True  rp", trueRp);
    print(&ss1, "True |rp|", trueRp.Magnitude());
    bar(&ss1);
    print(&ss1, "Calc  rp", computedRp);
    print(&ss1, "Calc |rp|", computedRp.Magnitude());
    bar(&ss1);
    print(&ss1, "Diff", trueRp.Magnitude()-computedRp.Magnitude());
    bar(&ss1);
    print(&ss1, "Actual rc", actual);
    print(&ss1, "Expected rc", expected);
    print(&ss1,"\n");
    LOG_INFO(ss1.str());
    EXPECT_LT(abs(trueRp.Magnitude() - computedRp.Magnitude()), 0.0001);
    VECTOR_EQUALS(actual, expected, BALLPARK);
}

TEST(SpheroidDistanceDeterminationAlgorithmTest, TestRandomEarth2) {
    Vec3 principleAxisDimensions = {RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_C};
    int imageWidth = 700;
    int imageHeight = 600;
    Camera cam(0.05, .0000514, imageWidth, imageHeight); 
    Vec3 AOR = {-0.42324764, -0.1952689,  -0.88472114};
    Points pts = {{static_cast<decimal>(576.4257627753286), static_cast<decimal>(598.7430618240066)},{static_cast<decimal>(661.6541409018258), static_cast<decimal>(529.7570867919333)},{static_cast<decimal>(588.0635060034172), static_cast<decimal>(589.777407622001)},{static_cast<decimal>(652.9762999432182), static_cast<decimal>(537.1447405098314)},{static_cast<decimal>(654.8726756023539), static_cast<decimal>(535.5376185395149)},{static_cast<decimal>(596.8277951206118), static_cast<decimal>(582.9335882529601)},{static_cast<decimal>(675.7904176672815), static_cast<decimal>(517.5374127325981)},{static_cast<decimal>(586.0334567507481), static_cast<decimal>(591.3513134961818)},{static_cast<decimal>(621.1805971719822), static_cast<decimal>(563.4936144300615)},{static_cast<decimal>(630.263881089284), static_cast<decimal>(556.0799787155689)},{static_cast<decimal>(608.1120899596912), static_cast<decimal>(574.0039264631828)},{static_cast<decimal>(615.7394551246949), static_cast<decimal>(567.8918454528773)},{static_cast<decimal>(649.2139282748626), static_cast<decimal>(540.3212122527914)},{static_cast<decimal>(636.8773833807785), static_cast<decimal>(550.6254141266548)},{static_cast<decimal>(628.9981942452558), static_cast<decimal>(557.118398931639)},{static_cast<decimal>(696.6164492479918), static_cast<decimal>(499.1081932263702)},{static_cast<decimal>(595.578519855075), static_cast<decimal>(583.9139850156122)},{static_cast<decimal>(590.0360864252954), static_cast<decimal>(588.243995953845)},{static_cast<decimal>(589.0709144084918), static_cast<decimal>(588.9947862041763)},{static_cast<decimal>(656.8422731356512), static_cast<decimal>(533.8641262088919)},{static_cast<decimal>(649.1286677782289), static_cast<decimal>(540.393010658714)},{static_cast<decimal>(603.1160173223736), static_cast<decimal>(577.9740065868167)},{static_cast<decimal>(593.14236249107), static_cast<decimal>(585.8211469811814)},
};
    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), principleAxisDimensions, AOR);
    Vec3 actual = algo.Run(pts);
    Vec3 expected = {2934.087845016932, 3818.970257548237, -5209.988096138158};

    Vec3 trueRp = {1395.584236904298, -6443.276615765561, 2621.8147079167147};

    Mat3 computedTCP = algo.ComputeBodyToCamTransformation(AOR).Transpose();
    Vec3 computedRp = computedTCP * actual;

    const Mat3 Ap = {1/(RADIUS_OF_EARTH_A*RADIUS_OF_EARTH_A),0,0,
                              0,1/(RADIUS_OF_EARTH_A*RADIUS_OF_EARTH_A),0,
                              0,0,1/(RADIUS_OF_EARTH_C*RADIUS_OF_EARTH_C)};

    const Mat3 Ac = computedTCP * Ap * computedTCP.Transpose();

    std::stringstream ss1;
    print(&ss1,"\n");
    conicEquation(&ss1, actual, Ac);
    print(&ss1, "TCP Det  ", computedTCP.Det());
    bar(&ss1);
    print(&ss1, "True  rp", trueRp);
    print(&ss1, "True |rp|", trueRp.Magnitude());
    bar(&ss1);
    print(&ss1, "Calc  rp", computedRp);
    print(&ss1, "Calc |rp|", computedRp.Magnitude());
    bar(&ss1);
    print(&ss1, "Diff", trueRp.Magnitude()-computedRp.Magnitude());
    bar(&ss1);
    print(&ss1, "Actual rc", actual);
    print(&ss1, "Expected rc", expected);
    print(&ss1,"\n");
    LOG_INFO(ss1.str());
    EXPECT_LT(abs(trueRp.Magnitude() - computedRp.Magnitude()), 0.0001);
    VECTOR_EQUALS(actual, expected, BALLPARK);
}