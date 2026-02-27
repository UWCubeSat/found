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

void conicEquation(std::stringstream * ss, Vec3 rc, Mat3 Ac, Mat3 calibrator){
    Mat3 C = (Ac * rc.OuterProduct(rc) * (Ac)) + Ac*((rc.OuterProduct(Ac)*rc-1)*-1);
    C = calibrator.Transpose() * C * calibrator;
    
    C = C * (1/C.At(0,0));
    *ss << "\nConic Equation:\n" << C.At(0,0) << "x^2 + " << C.At(0,1) << "*2xy + " << C.At(1,1) << "*y^2 + " << C.At(0,2) <<"*2x + " << C.At(1,2) << "*2y+ " << C.At(2,2) << " = 0";
}



TEST(SpheroidDistanceDeterminationAlgorithmTest, TestRandomEarth1) {
    Vec3 principleAxisDimensions = {RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_C};
    int imageWidth = 700;
    int imageHeight = 600;
    Camera cam(0.05, .0000514, imageWidth, imageHeight); 
    Vec3 attitude = {-36.3387, -151.885, 279.474};
    Points pts = {{static_cast<decimal>(611.7877295393952), static_cast<decimal>(264.09276117927675)},{static_cast<decimal>(573.8922119671513), static_cast<decimal>(325.3968742337466)},{static_cast<decimal>(350.4939091109065), static_cast<decimal>(589.3274589318719)},{static_cast<decimal>(391.14346133188616), static_cast<decimal>(550.233909535328)},{static_cast<decimal>(464.0968679643527), static_cast<decimal>(471.5734768209798)},{static_cast<decimal>(454.4893660268322), static_cast<decimal>(482.63970990423604)},{static_cast<decimal>(658.5589230687222), static_cast<decimal>(177.48386735439033)},{static_cast<decimal>(373.4109900481945), static_cast<decimal>(567.6612715009444)},{static_cast<decimal>(684.6437823193647), static_cast<decimal>(122.43996936965047)},{static_cast<decimal>(479.9742150253835), static_cast<decimal>(452.75848062170667)},{static_cast<decimal>(518.2721763857571), static_cast<decimal>(404.41735985920894)},{static_cast<decimal>(491.85561942267265), static_cast<decimal>(438.2265392379015)},{static_cast<decimal>(639.7657274333935), static_cast<decimal>(213.9521877859907)},{static_cast<decimal>(694.1329604135699), static_cast<decimal>(100.94348878568302)},{static_cast<decimal>(571.5349509252572), static_cast<decimal>(328.99120144536215)},{static_cast<decimal>(379.6326629337276), static_cast<decimal>(561.6146678003332)},{static_cast<decimal>(513.780424677385), static_cast<decimal>(410.31951218219683)},{static_cast<decimal>(655.9153967653705), static_cast<decimal>(182.76347341186144)},{static_cast<decimal>(340.53686956204075), static_cast<decimal>(598.4563901280716)},{static_cast<decimal>(392.5920815577219), static_cast<decimal>(548.783429938299)},{static_cast<decimal>(443.72986548528297), static_cast<decimal>(494.76101854907455)},{static_cast<decimal>(470.3974489413311), static_cast<decimal>(464.18744060291704)},{static_cast<decimal>(482.12392335127174), static_cast<decimal>(450.15862072244306)},
};
    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), principleAxisDimensions, attitude);
    Vec3 actual = algo.Run(pts);
    Vec3 expected = {4211.974212862954, 3307.209203472482, -5616.06183322837};

    Vec3 trueRp = {-5875.0, 4366.0, 2577.0};

    Mat3 computedTCP = algo.ComputeBodyToCamTransformation(attitude).Transpose();
    Vec3 computedRp = computedTCP * actual;

    const Mat3 Ap = {1/(RADIUS_OF_EARTH_A*RADIUS_OF_EARTH_A),0,0,
                              0,1/(RADIUS_OF_EARTH_A*RADIUS_OF_EARTH_A),0,
                              0,0,1/(RADIUS_OF_EARTH_C*RADIUS_OF_EARTH_C)};

    const Mat3 Ac = computedTCP * Ap * computedTCP.Transpose();
    const Mat3 Kinv = algo.ComputeInvCameraProjMat(cam);

    std::stringstream ss1;
    print(&ss1,"\n");
    conicEquation(&ss1, actual, Ac, Kinv);
    print(&ss1, "Ap Det   ", Ap.Det());
    print(&ss1, "Ac Det   ", Ac.Det());
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
    Vec3 attitude = {-36.3387, -151.885, 279.474};
    Points pts = {{static_cast<decimal>(34.76463980585136), static_cast<decimal>(597.8739266215779)},{static_cast<decimal>(582.5308475606472), static_cast<decimal>(129.62285813292584)},{static_cast<decimal>(197.3089107481149), static_cast<decimal>(529.2449401255765)},{static_cast<decimal>(82.25273009621013), static_cast<decimal>(581.2069163973963)},{static_cast<decimal>(300.78355286899705), static_cast<decimal>(465.1797275003244)},{static_cast<decimal>(50.41720562650499), static_cast<decimal>(592.6587089362533)},{static_cast<decimal>(372.4559127380931), static_cast<decimal>(408.052930850121)},{static_cast<decimal>(272.04286895095527), static_cast<decimal>(484.9198630296341)},{static_cast<decimal>(84.35279521431735), static_cast<decimal>(580.4103830053642)},{static_cast<decimal>(545.0314558544549), static_cast<decimal>(199.08139861644824)},{static_cast<decimal>(496.73090502573774), static_cast<decimal>(271.9018068561559)},{static_cast<decimal>(331.0456599288717), static_cast<decimal>(442.53358875572115)},{static_cast<decimal>(622.1876024795855), static_cast<decimal>(36.249463292078346)},{static_cast<decimal>(350.26184342327224), static_cast<decimal>(427.0669287177318)},{static_cast<decimal>(253.74974537323828), static_cast<decimal>(496.6608033888218)},{static_cast<decimal>(421.74216655337466), static_cast<decimal>(360.80939213313843)},{static_cast<decimal>(134.21613661516048), static_cast<decimal>(559.9157960021104)},{static_cast<decimal>(122.29848123578394), static_cast<decimal>(565.0980907781295)},{static_cast<decimal>(474.9261551394101), static_cast<decimal>(300.3061263202878)},{static_cast<decimal>(150.4985406172284), static_cast<decimal>(552.5330181217788)},{static_cast<decimal>(241.94320233051377), static_cast<decimal>(503.91854713164)},{static_cast<decimal>(590.6418505096397), static_cast<decimal>(112.54823935794057)},{static_cast<decimal>(301.5198771990676), static_cast<decimal>(464.65213074618856)},
};
    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), principleAxisDimensions, attitude);
    Vec3 actual = algo.Run(pts);
    Vec3 expected = {4029.3377696517746, 3966.982445499629, -7692.626122798081};

    Vec3 trueRp = {-7940.0, 4173.3, 3269.4};

    Mat3 computedTCP = algo.ComputeBodyToCamTransformation(attitude).Transpose();
    Vec3 computedRp = computedTCP * actual;

    const Mat3 Ap = {1/(RADIUS_OF_EARTH_A*RADIUS_OF_EARTH_A),0,0,
                              0,1/(RADIUS_OF_EARTH_A*RADIUS_OF_EARTH_A),0,
                              0,0,1/(RADIUS_OF_EARTH_C*RADIUS_OF_EARTH_C)};

    const Mat3 Ac = computedTCP * Ap * computedTCP.Transpose();
    const Mat3 Kinv = algo.ComputeInvCameraProjMat(cam);

    std::stringstream ss1;
    print(&ss1,"\n");
    conicEquation(&ss1, actual, Ac, Kinv);
    print(&ss1, "Ap Det   ", Ap.Det());
    print(&ss1, "Ac Det   ", Ac.Det());
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

TEST(SpheroidDistanceDeterminationAlgorithmTest, TestRandomEarth3) {
    Vec3 principleAxisDimensions = {RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_C};
    int imageWidth = 700;
    int imageHeight = 600;
    Camera cam(0.05, .0000514, imageWidth, imageHeight); 
    Vec3 attitude = {-31.772, -41.119, 163.031};
    Points pts = {{static_cast<decimal>(246.42904247369944), static_cast<decimal>(312.32022197566454)},{static_cast<decimal>(99.40636153091175), static_cast<decimal>(258.7374183079184)},{static_cast<decimal>(166.90042879347595), static_cast<decimal>(279.6321364042625)},{static_cast<decimal>(583.2996599930879), static_cast<decimal>(597.2301412764596)},{static_cast<decimal>(437.9909243792035), static_cast<decimal>(437.2250516685504)},{static_cast<decimal>(310.68670435324475), static_cast<decimal>(346.0948608167793)},{static_cast<decimal>(293.79194001181537), static_cast<decimal>(336.5152079793718)},{static_cast<decimal>(338.42615496062587), static_cast<decimal>(362.98892824521465)},{static_cast<decimal>(481.17933803836576), static_cast<decimal>(477.26747101989224)},{static_cast<decimal>(195.10344964264274), static_cast<decimal>(290.1629053536276)},{static_cast<decimal>(99.11172499568237), static_cast<decimal>(258.65876520441105)},{static_cast<decimal>(131.7554858410619), static_cast<decimal>(268.0231809130424)},{static_cast<decimal>(289.3745642415664), static_cast<decimal>(334.0955596841298)},{static_cast<decimal>(17.043174263128524), static_cast<decimal>(240.65544411039244)},{static_cast<decimal>(6.274549580548361), static_cast<decimal>(238.84271035632648)},{static_cast<decimal>(308.2336838995345), static_cast<decimal>(344.67144187370616)},{static_cast<decimal>(427.2759172676097), static_cast<decimal>(428.09476246855957)},{static_cast<decimal>(104.35971451272322), static_cast<decimal>(260.0754663385196)},{static_cast<decimal>(63.43021762656365), static_cast<decimal>(249.89384455634712)},{static_cast<decimal>(445.88998718338814), static_cast<decimal>(444.1524358798795)},{static_cast<decimal>(97.16650324738355), static_cast<decimal>(258.1421184938612)},{static_cast<decimal>(234.2409036489677), static_cast<decimal>(306.69246588565983)},{static_cast<decimal>(68.89860623878637), static_cast<decimal>(251.14091454700184)},
};
    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), principleAxisDimensions, attitude);
    Vec3 actual = algo.Run(pts);
    Vec3 expected = {3514.0819519351016, -5839.822334611661, -7030.609398569573};

    Vec3 trueRp = {-4139.9, 8662.1, 1926.4};

    Mat3 computedTCP = algo.ComputeBodyToCamTransformation(attitude).Transpose();
    Vec3 computedRp = computedTCP * actual;

    const Mat3 Ap = {1/(RADIUS_OF_EARTH_A*RADIUS_OF_EARTH_A),0,0,
                              0,1/(RADIUS_OF_EARTH_A*RADIUS_OF_EARTH_A),0,
                              0,0,1/(RADIUS_OF_EARTH_C*RADIUS_OF_EARTH_C)};

    const Mat3 Ac = computedTCP * Ap * computedTCP.Transpose();
    const Mat3 Kinv = algo.ComputeInvCameraProjMat(cam);

    std::stringstream ss1;
    print(&ss1,"\n");
    conicEquation(&ss1, actual, Ac, Kinv);
    print(&ss1, "Ap Det   ", Ap.Det());
    print(&ss1, "Ac Det   ", Ac.Det());
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