#include <gtest/gtest.h>

#include <cmath>

#include "common/style.hpp"
#include "common/spatial/attitude-utils.hpp"
#include "common/spatial/camera.hpp"
#include "distance/distance.hpp"
#include "common/logging.hpp"
#include "common/decimal.hpp"


/* Using Directives */
using found::Camera;
using found::Vec3;
using found::Points;
using found::PositionVector;
using found::SphericalDistanceDeterminationAlgorithm;


/* Common Constants */


// Radius of Earth (m)
#define RADIUS_OF_EARTH (static_cast<decimal>(6378137.0))
// Default DoubleEquals Tolerance (So big because of floating point problems)
#define DEFAULT_TOLERANCE 0.01
// Tolerance for Generator-Derived Points


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

// Base Case I: The image captured contains an edge centered about the image

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthX1) {
    // Step I: Pick some distance (m) and a Camera
    decimal x_E = RADIUS_OF_EARTH + 1000000;
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 1, imageWidth, imageHeight);  // Focal length of 12 m
    PositionVector expected = {x_E, 0, 0};

    // Step II: Figure out my projection points

    // a) Find the angle
    decimal alpha = asin(RADIUS_OF_EARTH / x_E);

    // b) Find the distance away from each projection point
    decimal p = sqrt(x_E * x_E - RADIUS_OF_EARTH * RADIUS_OF_EARTH);

    // c) Use 3 easy projections
    Vec3 p1 = {static_cast<decimal>(p * cos(alpha)), static_cast<decimal>(p * sin(alpha)), 0};
    Vec3 p2 = {static_cast<decimal>(p * cos(alpha)), static_cast<decimal>(-p * sin(alpha)), 0};
    Vec3 p3 = {static_cast<decimal>(p * cos(alpha)), 0, static_cast<decimal>(p * sin(alpha))};

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {cam.SpatialToCamera(p1),
                cam.SpatialToCamera(p2),
                cam.SpatialToCamera(p3)};

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);

    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthX2) {
    // Step I: Pick some distance (m) and a Camera
    decimal x_E = RADIUS_OF_EARTH + 10000000;
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {x_E, 0, 0};

    // Step II: Figure out my projection points

    // a) Find the angle
    decimal alpha = asin(RADIUS_OF_EARTH / x_E);

    // b) Find the distance away from each projection point
    decimal p = sqrt(x_E * x_E - RADIUS_OF_EARTH * RADIUS_OF_EARTH);
    decimal centerMag = static_cast<decimal>(p * cos(alpha));
    decimal projectionRadiusMag = static_cast<decimal>(p * sin(alpha));

    // c) Use 3 easy projections
    Vec3 p1 = {centerMag, projectionRadiusMag * -std::cos(0.1), projectionRadiusMag * std::sin(0.1)};
    Vec3 p2 = {centerMag, projectionRadiusMag, 0};
    Vec3 p3 = {centerMag, projectionRadiusMag * -std::cos(-0.1), projectionRadiusMag * std::sin(-0.1)};

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {cam.SpatialToCamera(p1),
                cam.SpatialToCamera(p2),
                cam.SpatialToCamera(p3)};

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);

    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthY1) {
    // Step I: Pick some distance (m) and a Camera
    decimal x_E = 7000000;
    int imageWidth = 6000;
    int imageHeight = 6000;
    Camera cam(0.085, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {0, x_E, 0};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
    {static_cast<decimal>(1040.29858052800545920035801827907562),
        static_cast<decimal>(2153.41717327522883351775817573070526)},
    {static_cast<decimal>(905.12458718505968136014416813850403),
        static_cast<decimal>(4843.01654323685215786099433898925781)},
    {static_cast<decimal>(649.50418565826112171635031700134277),
        static_cast<decimal>(5992.19055700358148897066712379455566)}
    };

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);

    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthY2) {
    // Step I: Pick some distance (m) and a Camera
    decimal x_E = 80000000;
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.0001, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {0, x_E, 0};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
    {static_cast<decimal>(15.92071472869832859942107461392879),
        static_cast<decimal>(551.36076804192668987525394186377525)},
    {static_cast<decimal>(383.64131687430347028566757217049599),
        static_cast<decimal>(520.96642804561145112529629841446877)},
    {static_cast<decimal>(425.31380021720195827583665959537029),
        static_cast<decimal>(516.80315691018540746881626546382904)}
    };

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);
    // actual = actual * static_cast<decimal>(0.00002);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthY3) {
    // Step 0: Determine Quaterion rotation
    found::Quaternion positionDirection = found::SphericalToQuaternion(
        static_cast<decimal> (M_PI / 2), static_cast<decimal>(0), static_cast<decimal>(0)).Conjugate();

    // Step I: Pick some distance (m) and a Camera
    decimal y_E = RADIUS_OF_EARTH + 10000000;
    int imageWidth = 100000;
    int imageHeight = 100000;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {0, y_E, 0};

    // Step II: Figure out my projection points

    // a) Find the angle
    decimal alpha = asin(RADIUS_OF_EARTH / y_E);

    // b) Find the distance away from each projection point
    decimal p = sqrt(y_E * y_E - RADIUS_OF_EARTH * RADIUS_OF_EARTH);
    decimal centerMag = static_cast<decimal>(p * cos(alpha));
    decimal projectionRadiusMag = static_cast<decimal>(p * sin(alpha));

    // c) Use 3 easy projections
    Vec3 p1 = {centerMag, -projectionRadiusMag * std::cos(0.1), projectionRadiusMag * std::sin(0.1)};
    Vec3 p2 = {centerMag, -projectionRadiusMag, 0};
    Vec3 p3 = {centerMag, -projectionRadiusMag * std::cos(-0.1), projectionRadiusMag * std::sin(-0.1)};

    // Logging information
    // std::stringstream ss1;

    // ss1 << p1.x << " " << p1.y << " " << p1.z;

    // LOG_INFO(ss1.str());

    Vec3 p1Rotated = positionDirection.Rotate(p1);
    Vec3 p2Rotated = positionDirection.Rotate(p2);
    Vec3 p3Rotated = positionDirection.Rotate(p3);

    // std::stringstream ss;
    // ss << p1Rotated.x << " " << p1Rotated.y << " " << p1Rotated.z;

    // LOG_INFO(ss.str());

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {cam.SpatialToCamera(p1Rotated),
                cam.SpatialToCamera(p2Rotated),
                cam.SpatialToCamera(p3Rotated)};

    // Points pts = {cam.SpatialToCamera(p1),
    //     cam.SpatialToCamera(p2),
    //     cam.SpatialToCamera(p3)};


    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);

    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {-34252, 7000000, -1345};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {static_cast<decimal>(161.20155091684796389017719775438309),
            static_cast<decimal>(991.52152895425956558028701692819595)},
        {static_cast<decimal>(235.83603309071037301691831089556217),
            static_cast<decimal>(570.37281705518421404121909290552139)},
        {static_cast<decimal>(202.39831757850390658859396353363991),
            static_cast<decimal>(199.33718626805284657166339457035065)}
    };

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom2) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {static_cast<decimal>(6902903.16156481951475143432617187500000),
                                static_cast<decimal>(4057963.08183827949687838554382324218750),
                                static_cast<decimal>(-123345.00000000002910383045673370361328)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {static_cast<decimal>(592.89445903633543366595404222607613),
            static_cast<decimal>(27.95577213927589355080272071063519)},
        {static_cast<decimal>(758.20232211636846386682009324431419),
            static_cast<decimal>(556.69381690731813705497188493609428)},
        {static_cast<decimal>(755.29136454972194769652560353279114),
            static_cast<decimal>(602.04870605634823732543736696243286)}
    };

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom3) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {static_cast<decimal>(7738762.75667632184922695159912109375000),
                               static_cast<decimal>(4615994.28818791918456554412841796875000),
                               static_cast<decimal>(-13345.00000000000000000000000000000000)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {static_cast<decimal>(481.84834444357670690806116908788681),
            static_cast<decimal>(964.81040225573292445915285497903824)},
        {static_cast<decimal>(621.97548491777308754535624757409096),
            static_cast<decimal>(737.08126743617572174116503447294235)},
        {static_cast<decimal>(428.66993432891945303708780556917191),
            static_cast<decimal>(1020.62451445330134447431191802024841)}
    };

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom4) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {static_cast<decimal>(87626881.87387187778949737548828125000000),
                               static_cast<decimal>(20684452.64770639687776565551757812500000),
                               static_cast<decimal>(-13345.00000000000363797880709171295166)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {static_cast<decimal>(401.28657374343384844905813224613667),
            static_cast<decimal>(543.19624806982301379321143031120300)},
        {static_cast<decimal>(341.96048250972955884208204224705696),
            static_cast<decimal>(546.63626918265651966066798195242882)},
        {static_cast<decimal>(414.20639106466398970951559022068977),
            static_cast<decimal>(517.96460526792134260176680982112885)}
    };

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom5) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {static_cast<decimal>(49882640.75393147766590118408203125000000),
                               static_cast<decimal>(18159469.50042396783828735351562500000000),
                               static_cast<decimal>(-45628454.04873351752758026123046875000000)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {static_cast<decimal>(366.92974981058114281040616333484650),
            static_cast<decimal>(1010.51278202486105328716803342103958)},
        {static_cast<decimal>(291.35363265088767548149917274713516),
            static_cast<decimal>(967.22909799014905729563906788825989)},
        {static_cast<decimal>(338.16847411711296444991603493690491),
            static_cast<decimal>(972.60153222357519098295597359538078)}
    };

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom6) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected ={static_cast<decimal>(54043203.25303997844457626342773437500000),
                              static_cast<decimal>(8435671.34863081201910972595214843750000),
                              static_cast<decimal>(-49841910.58559905737638473510742187500000)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {static_cast<decimal>(445.31674167128113595026661641895771),
            static_cast<decimal>(980.10941674657453859254019334912300)},
        {static_cast<decimal>(449.85090050041014819726115092635155),
            static_cast<decimal>(982.30626786287666618591174483299255)},
        {static_cast<decimal>(472.65429568256860193287138827145100),
            static_cast<decimal>(1003.57878217427446543297264724969864)}
    };

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom7) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected ={static_cast<decimal>(70446163.83446569740772247314453125000000),
                              static_cast<decimal>(32912272.59358813613653182983398437500000),
                              static_cast<decimal>(31783183.18714027479290962219238281250000)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {static_cast<decimal>(216.29029196725059591699391603469849),
            static_cast<decimal>(294.33566761979125203652074560523033)},
        {static_cast<decimal>(169.94983631433450455006095580756664),
            static_cast<decimal>(218.91908548044040117019903846085072)},
        {static_cast<decimal>(184.43271740825855431467061862349510),
            static_cast<decimal>(270.51356645365228814625879749655724)}
    };

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom8) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected ={static_cast<decimal>(70285818.03521062433719635009765625000000),
                              static_cast<decimal>(25857600.50544352829456329345703125000000),
                              static_cast<decimal>(38938641.89612446725368499755859375000000)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {static_cast<decimal>(242.84406646531311935177654959261417),
            static_cast<decimal>(205.47042245322984399535926058888435)},
        {static_cast<decimal>(344.91199409546220522315707057714462),
            static_cast<decimal>(205.76833406671806869780994020402431)},
        {static_cast<decimal>(231.67353495233982130230288021266460),
            static_cast<decimal>(158.12717606263504421804100275039673)}
    };

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom9) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected ={static_cast<decimal>(79999999.99999998509883880615234375000000),
                              static_cast<decimal>(-1233.99999998530392986140213906764984),
                              static_cast<decimal>(-44212.99999999998544808477163314819336)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {static_cast<decimal>(534.36078806397608786937780678272247),
            static_cast<decimal>(554.79942136584782019781414419412613)},
        {static_cast<decimal>(559.82106976178147306200116872787476),
            static_cast<decimal>(516.45194077039036528731230646371841)},
        {static_cast<decimal>(541.59569905005412238097051158547401),
            static_cast<decimal>(550.11691609693627924571046605706215)}
    };

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(SphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom10) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {static_cast<decimal>(80000000.00000000000000000000000000000000),
                               static_cast<decimal>(32135.88188458501463173888623714447021),
                               static_cast<decimal>(-30390.74234863133460748940706253051758)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {static_cast<decimal>(525.10096265445156404894078150391579),
            static_cast<decimal>(466.13302633119337770040147006511688)},
        {static_cast<decimal>(474.05885253783083044254453852772713),
            static_cast<decimal>(482.53527642185855484058265574276447)},
        {static_cast<decimal>(464.81533776455961515239323489367962),
            static_cast<decimal>(522.19750545507190508942585438489914)}
    };

    // Step IV: Run It and Test!
    SphericalDistanceDeterminationAlgorithm algo =
        SphericalDistanceDeterminationAlgorithm(RADIUS_OF_EARTH, cam);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}
