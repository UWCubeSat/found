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

#define BALLPARK 0.005 // we're within half a kilometer; Department of War is probably fine with that right

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
    ss1 << "\nExpected: [" << expected.x << ", "<< expected.y << ", " << expected.z << "]";
    ss1 << "\nActual: [" << actual.x << ", "<< actual.y << ", " << actual.z << "]";
    Vec3 rp = algo.ComputeBodyToCamTransformation(AOR) * actual;
    ss1 << "\nActual rp: [" << rp.x << ", "<< rp.y << ", " << rp.z << "]";
    LOG_INFO(ss1.str());

    VECTOR_EQUALS(expected, actual, BALLPARK);
}

TEST(SpheroidDistanceDeterminationAlgorithmTest, TestRandomEarth1) {
    Vec3 principleAxisDimensions = {RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_C};
    int imageWidth = 700;
    int imageHeight = 600;
    Camera cam(0.05, .0000514, imageWidth, imageHeight); 
    Vec3 AOR = {0.366283, 0.69847598, -0.61479108};
    Points pts = {
        {static_cast<decimal>(29.640154372346906), static_cast<decimal>(552.0730992967581)},
        {static_cast<decimal>(52.66802444095718), static_cast<decimal>(383.7849029013289)},
        {static_cast<decimal>(90.04864434133547), static_cast<decimal>(231.35406188055828)},
        {static_cast<decimal>(115.74687713551153), static_cast<decimal>(153.67726727159595)},
        {static_cast<decimal>(42.19012175638361), static_cast<decimal>(444.92039585584604)},
        {static_cast<decimal>(83.92305717242371), static_cast<decimal>(252.2754374167765)},
        {static_cast<decimal>(26.538220123274968), static_cast<decimal>(595.9289100323681)},
        {static_cast<decimal>(149.81559773385052), static_cast<decimal>(67.45175146967613)},
        {static_cast<decimal>(36.80786247700964), static_cast<decimal>(483.6814973453413)},
        {static_cast<decimal>(177.80014884141), static_cast<decimal>(6.124583976105158)}
    };


    
    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), principleAxisDimensions, AOR);
    Vec3 expected = {-4830.02027492 ,  -1482.55456426, -5651.36054838};
    Vec3 actual = algo.Run(pts);
    
    std::stringstream ss1;
    ss1 << "\nExpected: [" << expected.x << ", "<< expected.y << ", " << expected.z << "]";
    ss1 << "\nActual: [" << actual.x << ", "<< actual.y << ", " << actual.z << "]";
    Vec3 rp = algo.ComputeBodyToCamTransformation(AOR).Transpose() * actual;
    ss1 << "\nActual rp: [" << rp.x << ", "<< rp.y << ", " << rp.z << "]";

    Mat3 BlenderTPC = { 0.88566719, -0.28536013,  0.366283,  
                        -0.05907825,  0.71319076,  0.69847598,
                        0.46054684,  0.64025661, -0.61479108};
    Vec3 blenderRp = BlenderTPC * actual* 0.001;
    ss1 << "\nBlender rp: [" << blenderRp.x << ", "<< blenderRp.y << ", " << blenderRp.z << "]"; // Putting this into blender gives us the same conic that we see in desmos; the algorithm works but the generator is wrong
    LOG_INFO(ss1.str());

    VECTOR_EQUALS(expected, actual, BALLPARK);
}

TEST(SpheroidDistanceDeterminationAlgorithmTest, TestRandomEarth2) {
    Vec3 principleAxisDimensions = {RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_C};
    int imageWidth = 700;
    int imageHeight = 600;
    Camera cam(0.05, .0000514, imageWidth, imageHeight); 
    Vec3 AOR = {-0.689832, -0.717795,  0.09434};
    Points pts = {
        {static_cast<decimal>(666.0205755805313), static_cast<decimal>(589.68526610393)},
{static_cast<decimal>(694.6464109502064), static_cast<decimal>(573.1051279383721)},
{static_cast<decimal>(656.7934499048628), static_cast<decimal>(594.783075932027)},
{static_cast<decimal>(685.016012956038), static_cast<decimal>(578.8126631417758)},
{static_cast<decimal>(650.3816817607858), static_cast<decimal>(598.2596989807637)},
{static_cast<decimal>(677.1625620924857), static_cast<decimal>(583.3750002837222)},
{static_cast<decimal>(692.6906647868942), static_cast<decimal>(574.2721197464522)},
{static_cast<decimal>(686.2085157919271), static_cast<decimal>(578.1186411995184)},
{static_cast<decimal>(694.6849577640263), static_cast<decimal>(573.0815527806068)},
{static_cast<decimal>(660.6841185075613), static_cast<decimal>(592.6557011054858)},
    };


    
    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), principleAxisDimensions, AOR);
    Vec3 expected = {-1070.6298, -3489.8742,  7944.1600,};
    Vec3 actual = algo.Run(pts);
    
    std::stringstream ss1;
    ss1 << "\nExpected: [" << expected.x << ", "<< expected.y << ", " << expected.z << "]";
    ss1 << "\nActual: [" << actual.x << ", "<< actual.y << ", " << actual.z << "]";
    Vec3 rp = algo.ComputeBodyToCamTransformation(AOR).Transpose() * actual;
    ss1 << "\nActual rp: [" << rp.x << ", "<< rp.y << ", " << rp.z << "]";

    Mat3 BlenderTPC = { -0.72395133,  0.00507139, -0.68983241,  
                       0.68484612,  0.12551973, -0.71779564,
                       -0.08294735,  0.99207816,  0.0943433};
    Vec3 blenderRp = BlenderTPC * actual* 0.001;
    ss1 << "\nBlender rp: [" << blenderRp.x << ", "<< blenderRp.y << ", " << blenderRp.z << "]"; // Putting this into blender gives us the same conic that we see in desmos; the algorithm works but the generator is wrong
    LOG_INFO(ss1.str());

    VECTOR_EQUALS(expected, actual, BALLPARK);
}

TEST(SpheroidDistanceDeterminationAlgorithmTest, TestRandomEarth3) {
    Vec3 principleAxisDimensions = {RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_A, RADIUS_OF_EARTH_C};
    int imageWidth = 700;
    int imageHeight = 600;
    Camera cam(0.05, .0000514, imageWidth, imageHeight); 
    Vec3 AOR = {0.077951,   -0.99272918,  0.09171918};
    Points pts = {
        {static_cast<decimal>(654.6), static_cast<decimal>(221)},
        {static_cast<decimal>(506), static_cast<decimal>(268.9)},
        {static_cast<decimal>(594.6), static_cast<decimal>(243.6)},
        {static_cast<decimal>(350), static_cast<decimal>(290)},
        {static_cast<decimal>(418.7), static_cast<decimal>(285)},
        {static_cast<decimal>(115), static_cast<decimal>(269)},
        {static_cast<decimal>(263.2), static_cast<decimal>(290.7)},
        {static_cast<decimal>(227), static_cast<decimal>(287.7)}
    };


    
    SpheroidDistanceDeterminationAlgorithm algo(std::move(cam), principleAxisDimensions, AOR);
    Vec3 actual = algo.Run(pts);
    
    std::stringstream ss1;
    ss1 << "\nActual: [" << actual.x << ", "<< actual.y << ", " << actual.z << "]";
    Vec3 rp = algo.ComputeBodyToCamTransformation(AOR).Transpose() * actual;
    ss1 << "\nActual rp: [" << rp.x << ", "<< rp.y << ", " << rp.z << "]";

    Mat3 BlenderTPC = { -0.98422668,  0.1588127,   0.077951,  
                        -0.06197403,  0.10318908, -0.99272918,
                        0.1657017,   0.98190149,  0.09171918};
    Vec3 actualBlenderRp = BlenderTPC * actual * 0.001;
    ss1 << "\nBlender rp: [" << actualBlenderRp.x << ", "<< actualBlenderRp.y << ", " << actualBlenderRp.z << "]";
    Vec3 expectedBlenderRp = {0.180443, 7.83909, 5.69089};
    LOG_INFO(ss1.str());

    VECTOR_EQUALS(expectedBlenderRp, actualBlenderRp, BALLPARK);
}