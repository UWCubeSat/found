#include <gtest/gtest.h>

#include <cmath>
#include <utility>

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
using found::IterativeSphericalDistanceDeterminationAlgorithm;


/* Common Constants */


// Radius of Earth (m)
#define RADIUS_OF_EARTH (DECIMAL(6378137.0))
// Default DoubleEquals Tolerance (So big for float because of floating point problems)
#ifdef FOUND_FLOAT_MODE
    #define DEFAULT_TOLERANCE DECIMAL(2000.0)
#else
    #include "test/common/common.hpp"
#endif
// Default Distance Tolerance (m)
#define DEFAULT_DIST_TOL 10
// Default Discriminator Ratio
#define DEFAULT_DISC_RAT 1.1
// Default PDF Order
#define DEFAULT_PDF_ORD 2
// Defualt Radius Loss Order
#define DEFAULT_R_L_ORD 4

// Default Iterations
#define DEFAULT_ITERATIONS_1 1
#define DEFAULT_ITERATIONS_2 2

// Default Refreshes
#define DEFAULT_REFRESHES_0 0
#define DEFAULT_REFRESHES_1 1


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
    EXPECT_LT(abs(vec1.x() - vec2.x()), tolerance); \
    EXPECT_LT(abs(vec1.y() - vec2.y()), tolerance); \
    EXPECT_LT(abs(vec1.z() - vec2.z()), tolerance);

TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestConstructorLowPowers) {
    // Taken from TestCenteredEarthRandom7
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected ={DECIMAL(70446163.83446569740772247314453125000000),
                              DECIMAL(32912272.59358813613653182983398437500000),
                              DECIMAL(31783183.18714027479290962219238281250000)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {DECIMAL(216.29029196725059591699391603469849),
            DECIMAL(294.33566761979125203652074560523033)},
        {DECIMAL(169.94983631433450455006095580756664),
            DECIMAL(218.91908548044040117019903846085072)},
        {DECIMAL(184.43271740825855431467061862349510),
            DECIMAL(270.51356645365228814625879749655724)}
    };

    // Step IV: Inject bad powers that will cause failure when handling
    // negative inputs
    IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                          std::move(cam),
                                                          DEFAULT_ITERATIONS_2,
                                                          DEFAULT_REFRESHES_0,
                                                          DEFAULT_DIST_TOL,
                                                          DEFAULT_DISC_RAT,
                                                          1,
                                                          -3);

    // Testing that it still physically works
    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestNotEnoughPoints) {
    // Pick any initialization
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 1, imageWidth, imageHeight);
    PositionVector expected = {0.0, 0.0, 0.0};

    // Pick any vectors
    Vec3 p1 = {10, -26, 0};
    Vec3 p2 = {27, 93, -62};
    Points pts = {cam.CameraToPixelCoordinates(p1),
                cam.CameraToPixelCoordinates(p2)};

    // Run the algorithm
    IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                          std::move(cam),
                                                          DEFAULT_ITERATIONS_1,
                                                          DEFAULT_REFRESHES_1,
                                                          DEFAULT_DIST_TOL,
                                                          DEFAULT_DISC_RAT,
                                                          DEFAULT_PDF_ORD,
                                                          DEFAULT_R_L_ORD);
    PositionVector actual = algo.Run(pts);

    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestMoreThan3Points) {
    // Step I: Pick some distance (m) and a Camera
    decimal x_E = RADIUS_OF_EARTH + 10000000;
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {x_E, 0, 0};

    // Step II: Figure out my projection points

    // a) Find the angle
    decimal alpha = DECIMAL_ASIN(RADIUS_OF_EARTH / x_E);

    // b) Find the distance away from each projection point
    decimal p = sqrt(x_E * x_E - RADIUS_OF_EARTH * RADIUS_OF_EARTH);
    decimal centerMag = DECIMAL(p * DECIMAL_COS(alpha));
    decimal projectionRadiusMag = DECIMAL(p * DECIMAL_SIN(alpha));

    // c) Use 3 easy projections
    Vec3 p1 = {centerMag, projectionRadiusMag * -DECIMAL_COS(4.1), projectionRadiusMag * DECIMAL_SIN(4.1)};
    Vec3 p2 = {centerMag, projectionRadiusMag, 0};
    Vec3 p3 = {centerMag, projectionRadiusMag * -DECIMAL_COS(-3.1), projectionRadiusMag * DECIMAL_SIN(-3.1)};
    Vec3 p4 = {centerMag, projectionRadiusMag * -DECIMAL_COS(0.5), projectionRadiusMag * DECIMAL_SIN(0.5)};
    Vec3 p5 = {centerMag, projectionRadiusMag * -DECIMAL_COS(1.2), projectionRadiusMag * DECIMAL_SIN(1.2)};
    Vec3 p6 = {centerMag, projectionRadiusMag * -DECIMAL_COS(9.4), projectionRadiusMag * DECIMAL_SIN(9.4)};
    Vec3 p7 = {centerMag, projectionRadiusMag * -DECIMAL_COS(-62.4), projectionRadiusMag * DECIMAL_SIN(-62.4)};

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {cam.CameraToPixelCoordinates(p1),
                cam.CameraToPixelCoordinates(p2),
                cam.CameraToPixelCoordinates(p3),
                cam.CameraToPixelCoordinates(p4),
                cam.CameraToPixelCoordinates(p5),
                cam.CameraToPixelCoordinates(p6),
                cam.CameraToPixelCoordinates(p7)};

    // Step IV: Run It and Test!
    IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                          std::move(cam),
                                                          10000,
                                                          DEFAULT_REFRESHES_1,
                                                          DEFAULT_DIST_TOL,
                                                          DEFAULT_DISC_RAT,
                                                          DEFAULT_PDF_ORD,
                                                          DEFAULT_R_L_ORD);

    PositionVector actual = algo.Run(pts);

    #ifdef FOUND_FLOAT_MODE
        // For some reason, it gets very inaccurate, but just the x coordinate is off by
        // nearly 1200, which isn't a lot considering the scale. We use 1500 for safety.
        VECTOR_EQUALS(expected, actual, 1500);
    #else
        VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
    #endif
}

// Base Case I: The image captured contains an edge centered about the image

TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthX1) {
    // Step I: Pick some distance (m) and a Camera
    decimal x_E = RADIUS_OF_EARTH + 1000000;
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 1, imageWidth, imageHeight);  // Focal length of 12 m
    PositionVector expected = {x_E, 0, 0};

    // Step II: Figure out my projection points

    // a) Find the angle
    decimal alpha = DECIMAL_ASIN(RADIUS_OF_EARTH / x_E);

    // b) Find the distance away from each projection point
    decimal p = sqrt(x_E * x_E - RADIUS_OF_EARTH * RADIUS_OF_EARTH);

    // c) Use 3 easy projections
    Vec3 p1 = {DECIMAL(p * DECIMAL_COS(alpha)), DECIMAL(p * DECIMAL_SIN(alpha)), 0};
    Vec3 p2 = {DECIMAL(p * DECIMAL_COS(alpha)), DECIMAL(-p * DECIMAL_SIN(alpha)), 0};
    Vec3 p3 = {DECIMAL(p * DECIMAL_COS(alpha)), 0, DECIMAL(p * DECIMAL_SIN(alpha))};

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {cam.CameraToPixelCoordinates(p1),
                cam.CameraToPixelCoordinates(p2),
                cam.CameraToPixelCoordinates(p3)};

    // Step IV: Run It and Test!
    IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                          std::move(cam),
                                                          DEFAULT_ITERATIONS_1,
                                                          DEFAULT_REFRESHES_0,
                                                          DEFAULT_DIST_TOL,
                                                          DEFAULT_DISC_RAT,
                                                          DEFAULT_PDF_ORD,
                                                          DEFAULT_R_L_ORD);

    PositionVector actual = algo.Run(pts);

    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthX2) {
    // Step I: Pick some distance (m) and a Camera
    decimal x_E = RADIUS_OF_EARTH + 10000000;
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {x_E, 0, 0};

    // Step II: Figure out my projection points

    // a) Find the angle
    decimal alpha = DECIMAL_ASIN(RADIUS_OF_EARTH / x_E);

    // b) Find the distance away from each projection point
    decimal p = sqrt(x_E * x_E - RADIUS_OF_EARTH * RADIUS_OF_EARTH);
    decimal centerMag = DECIMAL(p * DECIMAL_COS(alpha));
    decimal projectionRadiusMag = DECIMAL(p * DECIMAL_SIN(alpha));

    // c) Use 3 easy projections
    Vec3 p1 = {centerMag, projectionRadiusMag * -DECIMAL_COS(0.1), projectionRadiusMag * DECIMAL_SIN(0.1)};
    Vec3 p2 = {centerMag, projectionRadiusMag, 0};
    Vec3 p3 = {centerMag, projectionRadiusMag * -DECIMAL_COS(-0.1), projectionRadiusMag * DECIMAL_SIN(-0.1)};

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {cam.CameraToPixelCoordinates(p1),
                cam.CameraToPixelCoordinates(p2),
                cam.CameraToPixelCoordinates(p3)};

    // Step IV: Run It and Test!
    IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                          std::move(cam),
                                                          DEFAULT_ITERATIONS_2,
                                                          DEFAULT_REFRESHES_1,
                                                          DEFAULT_DIST_TOL,
                                                          DEFAULT_DISC_RAT,
                                                          DEFAULT_PDF_ORD,
                                                          DEFAULT_R_L_ORD);

    PositionVector actual = algo.Run(pts);

    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthY1) {
    // Step I: Pick some distance (m) and a Camera
    decimal x_E = 7000000;
    int imageWidth = 6000;
    int imageHeight = 6000;
    Camera cam(0.085, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {0, x_E, 0};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
    {DECIMAL(1040.29858052800545920035801827907562),
        DECIMAL(2153.41717327522883351775817573070526)},
    {DECIMAL(905.12458718505968136014416813850403),
        DECIMAL(4843.01654323685215786099433898925781)},
    {DECIMAL(649.50418565826112171635031700134277),
        DECIMAL(5992.19055700358148897066712379455566)}
    };

    // Step IV: Run It and Test!
    IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                          std::move(cam),
                                                          DEFAULT_ITERATIONS_1,
                                                          DEFAULT_REFRESHES_0,
                                                          DEFAULT_DIST_TOL,
                                                          DEFAULT_DISC_RAT,
                                                          DEFAULT_PDF_ORD,
                                                          DEFAULT_R_L_ORD);

    PositionVector actual = algo.Run(pts);

    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthY2) {
    // Step I: Pick some distance (m) and a Camera
    decimal x_E = 80000000;
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.0001, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {0, x_E, 0};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
    {DECIMAL(15.92071472869832859942107461392879),
        DECIMAL(551.36076804192668987525394186377525)},
    {DECIMAL(383.64131687430347028566757217049599),
        DECIMAL(520.96642804561145112529629841446877)},
    {DECIMAL(425.31380021720195827583665959537029),
        DECIMAL(516.80315691018540746881626546382904)}
    };

    // Step IV: Run It and Test!
    IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                          std::move(cam),
                                                          DEFAULT_ITERATIONS_2,
                                                          DEFAULT_REFRESHES_1,
                                                          DEFAULT_DIST_TOL,
                                                          DEFAULT_DISC_RAT,
                                                          DEFAULT_PDF_ORD,
                                                          DEFAULT_R_L_ORD);

    PositionVector actual = algo.Run(pts);
    // actual = actual * DECIMAL(0.00002);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

#ifndef FOUND_FLOAT_MODE
    TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthY3) {
        // Step 0: Determine Quaterion rotation
        found::Quaternion positionDirection = found::SphericalToQuaternion(
            DECIMAL (M_PI / 2), DECIMAL(0), DECIMAL(0));

        // Step I: Pick some distance (m) and a Camera
        decimal y_E = RADIUS_OF_EARTH + 10000000;
        int imageWidth = 100000;
        int imageHeight = 100000;
        Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
        PositionVector expected = {0, y_E, 0};

        // Step II: Figure out my projection points

        // a) Find the angle
        decimal alpha = DECIMAL_ASIN(RADIUS_OF_EARTH / y_E);

        // b) Find the distance away from each projection point
        decimal p = sqrt(y_E * y_E - RADIUS_OF_EARTH * RADIUS_OF_EARTH);
        decimal centerMag = DECIMAL(p * DECIMAL_COS(alpha));
        decimal projectionRadiusMag = DECIMAL(p * DECIMAL_SIN(alpha));

        // c) Use 3 easy projections
        Vec3 p1 = {centerMag, -projectionRadiusMag * DECIMAL_COS(0.1), projectionRadiusMag * DECIMAL_SIN(0.1)};
        Vec3 p2 = {centerMag, -projectionRadiusMag, 0};
        Vec3 p3 = {centerMag, -projectionRadiusMag * DECIMAL_COS(-0.1), projectionRadiusMag * DECIMAL_SIN(-0.1)};

        Vec3 p1Rotated = positionDirection * p1;
        Vec3 p2Rotated = positionDirection * p2;
        Vec3 p3Rotated = positionDirection * p3;

        // Step III: Use CTS to convert to 2D vectors
        Points pts = {cam.CameraToPixelCoordinates(p1Rotated),
                    cam.CameraToPixelCoordinates(p2Rotated),
                    cam.CameraToPixelCoordinates(p3Rotated)};

        // Step IV: Run It and Test!
        IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                              std::move(cam),
                                                              DEFAULT_ITERATIONS_1,
                                                          DEFAULT_REFRESHES_0,
                                                              DEFAULT_DIST_TOL,
                                                              DEFAULT_DISC_RAT,
                                                              DEFAULT_PDF_ORD,
                                                              DEFAULT_R_L_ORD);

        PositionVector actual = algo.Run(pts);

        VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
    }
#endif

TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {-34252, 7000000, -1345};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {DECIMAL(161.20155091684796389017719775438309),
            DECIMAL(991.52152895425956558028701692819595)},
        {DECIMAL(235.83603309071037301691831089556217),
            DECIMAL(570.37281705518421404121909290552139)},
        {DECIMAL(202.39831757850390658859396353363991),
            DECIMAL(199.33718626805284657166339457035065)}
    };

    // Step IV: Run It and Test!
    IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                         std::move(cam),
                                                         DEFAULT_ITERATIONS_2,
                                                         DEFAULT_REFRESHES_1,
                                                         DEFAULT_DIST_TOL,
                                                         DEFAULT_DISC_RAT,
                                                         DEFAULT_PDF_ORD,
                                                         DEFAULT_R_L_ORD);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom2) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {DECIMAL(6902903.16156481951475143432617187500000),
                                DECIMAL(4057963.08183827949687838554382324218750),
                                DECIMAL(-123345.00000000002910383045673370361328)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {DECIMAL(592.89445903633543366595404222607613),
            DECIMAL(27.95577213927589355080272071063519)},
        {DECIMAL(758.20232211636846386682009324431419),
            DECIMAL(556.69381690731813705497188493609428)},
        {DECIMAL(755.29136454972194769652560353279114),
            DECIMAL(602.04870605634823732543736696243286)}
    };

    // Step IV: Run It and Test!
    IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                          std::move(cam),
                                                          DEFAULT_ITERATIONS_1,
                                                          DEFAULT_REFRESHES_0,
                                                          DEFAULT_DIST_TOL,
                                                          DEFAULT_DISC_RAT,
                                                          DEFAULT_PDF_ORD,
                                                          DEFAULT_R_L_ORD);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom3) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {DECIMAL(7738762.75667632184922695159912109375000),
                               DECIMAL(4615994.28818791918456554412841796875000),
                               DECIMAL(-13345.00000000000000000000000000000000)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {DECIMAL(481.84834444357670690806116908788681),
            DECIMAL(964.81040225573292445915285497903824)},
        {DECIMAL(621.97548491777308754535624757409096),
            DECIMAL(737.08126743617572174116503447294235)},
        {DECIMAL(428.66993432891945303708780556917191),
            DECIMAL(1020.62451445330134447431191802024841)}
    };

    // Step IV: Run It and Test!
    IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                          std::move(cam),
                                                          DEFAULT_ITERATIONS_2,
                                                          DEFAULT_REFRESHES_1,
                                                          DEFAULT_DIST_TOL,
                                                          DEFAULT_DISC_RAT,
                                                          DEFAULT_PDF_ORD,
                                                          DEFAULT_R_L_ORD);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom4) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {DECIMAL(87626881.87387187778949737548828125000000),
                               DECIMAL(20684452.64770639687776565551757812500000),
                               DECIMAL(-13345.00000000000363797880709171295166)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {DECIMAL(401.28657374343384844905813224613667),
            DECIMAL(543.19624806982301379321143031120300)},
        {DECIMAL(341.96048250972955884208204224705696),
            DECIMAL(546.63626918265651966066798195242882)},
        {DECIMAL(414.20639106466398970951559022068977),
            DECIMAL(517.96460526792134260176680982112885)}
    };

    // Step IV: Run It and Test!
    IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                          std::move(cam),
                                                          DEFAULT_ITERATIONS_1,
                                                          DEFAULT_REFRESHES_0,
                                                          DEFAULT_DIST_TOL,
                                                          DEFAULT_DISC_RAT,
                                                          DEFAULT_PDF_ORD,
                                                          DEFAULT_R_L_ORD);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom5) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {DECIMAL(49882640.75393147766590118408203125000000),
                               DECIMAL(18159469.50042396783828735351562500000000),
                               DECIMAL(-45628454.04873351752758026123046875000000)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {DECIMAL(366.92974981058114281040616333484650),
            DECIMAL(1010.51278202486105328716803342103958)},
        {DECIMAL(291.35363265088767548149917274713516),
            DECIMAL(967.22909799014905729563906788825989)},
        {DECIMAL(338.16847411711296444991603493690491),
            DECIMAL(972.60153222357519098295597359538078)}
    };

    // Step IV: Run It and Test!
    IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                          std::move(cam),
                                                          DEFAULT_ITERATIONS_2,
                                                          DEFAULT_REFRESHES_1,
                                                          DEFAULT_DIST_TOL,
                                                          DEFAULT_DISC_RAT,
                                                          DEFAULT_PDF_ORD,
                                                          DEFAULT_R_L_ORD);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

#ifndef FOUND_FLOAT_MODE
    TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom6) {
        // Step I: Pick some distance (m) and a Camera
        int imageWidth = 1024;
        int imageHeight = 1024;
        Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
        PositionVector expected ={DECIMAL(54043203.25303997844457626342773437500000),
                                DECIMAL(8435671.34863081201910972595214843750000),
                                DECIMAL(-49841910.58559905737638473510742187500000)};

        // Step II: Figure out my projection points

        // Step III: Use CTS to convert to 2D vectors
        Points pts = {
            {DECIMAL(445.31674167128113595026661641895771),
                DECIMAL(980.10941674657453859254019334912300)},
            {DECIMAL(449.85090050041014819726115092635155),
                DECIMAL(982.30626786287666618591174483299255)},
            {DECIMAL(472.65429568256860193287138827145100),
                DECIMAL(1003.57878217427446543297264724969864)}
        };

        // Step IV: Run It and Test!
        IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                          std::move(cam),
                                                          DEFAULT_ITERATIONS_1,
                                                          DEFAULT_REFRESHES_0,
                                                          DEFAULT_DIST_TOL,
                                                          DEFAULT_DISC_RAT,
                                                          DEFAULT_PDF_ORD,
                                                          DEFAULT_R_L_ORD);

        PositionVector actual = algo.Run(pts);
        VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
    }
#endif

TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom7) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected ={DECIMAL(70446163.83446569740772247314453125000000),
                              DECIMAL(32912272.59358813613653182983398437500000),
                              DECIMAL(31783183.18714027479290962219238281250000)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {DECIMAL(216.29029196725059591699391603469849),
            DECIMAL(294.33566761979125203652074560523033)},
        {DECIMAL(169.94983631433450455006095580756664),
            DECIMAL(218.91908548044040117019903846085072)},
        {DECIMAL(184.43271740825855431467061862349510),
            DECIMAL(270.51356645365228814625879749655724)}
    };

    // Step IV: Run It and Test!
    IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                          std::move(cam),
                                                          DEFAULT_ITERATIONS_2,
                                                          DEFAULT_REFRESHES_0,
                                                          DEFAULT_DIST_TOL,
                                                          DEFAULT_DISC_RAT,
                                                          DEFAULT_PDF_ORD,
                                                          DEFAULT_R_L_ORD);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom8) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected ={DECIMAL(70285818.03521062433719635009765625000000),
                              DECIMAL(25857600.50544352829456329345703125000000),
                              DECIMAL(38938641.89612446725368499755859375000000)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {DECIMAL(242.84406646531311935177654959261417),
            DECIMAL(205.47042245322984399535926058888435)},
        {DECIMAL(344.91199409546220522315707057714462),
            DECIMAL(205.76833406671806869780994020402431)},
        {DECIMAL(231.67353495233982130230288021266460),
            DECIMAL(158.12717606263504421804100275039673)}
    };

    // Step IV: Run It and Test!
    IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                          std::move(cam),
                                                          DEFAULT_ITERATIONS_1,
                                                          DEFAULT_REFRESHES_0,
                                                          DEFAULT_DIST_TOL,
                                                          DEFAULT_DISC_RAT,
                                                          DEFAULT_PDF_ORD,
                                                          DEFAULT_R_L_ORD);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom9) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected ={DECIMAL(79999999.99999998509883880615234375000000),
                              DECIMAL(-1233.99999998530392986140213906764984),
                              DECIMAL(-44212.99999999998544808477163314819336)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {DECIMAL(534.36078806397608786937780678272247),
            DECIMAL(554.79942136584782019781414419412613)},
        {DECIMAL(559.82106976178147306200116872787476),
            DECIMAL(516.45194077039036528731230646371841)},
        {DECIMAL(541.59569905005412238097051158547401),
            DECIMAL(550.11691609693627924571046605706215)}
    };

    // Step IV: Run It and Test!
    IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                          std::move(cam),
                                                          DEFAULT_ITERATIONS_2,
                                                          DEFAULT_REFRESHES_1,
                                                          DEFAULT_DIST_TOL,
                                                          DEFAULT_DISC_RAT,
                                                          DEFAULT_PDF_ORD,
                                                          DEFAULT_R_L_ORD);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}

TEST(IterativeSphericalDistanceDeterminationAlgorithmTest, TestCenteredEarthRandom10) {
    // Step I: Pick some distance (m) and a Camera
    int imageWidth = 1024;
    int imageHeight = 1024;
    Camera cam(0.012, 0.00002, imageWidth, imageHeight);  // Focal length of 12 mm
    PositionVector expected = {DECIMAL(80000000.00000000000000000000000000000000),
                               DECIMAL(32135.88188458501463173888623714447021),
                               DECIMAL(-30390.74234863133460748940706253051758)};

    // Step II: Figure out my projection points

    // Step III: Use CTS to convert to 2D vectors
    Points pts = {
        {DECIMAL(525.10096265445156404894078150391579),
            DECIMAL(466.13302633119337770040147006511688)},
        {DECIMAL(474.05885253783083044254453852772713),
            DECIMAL(482.53527642185855484058265574276447)},
        {DECIMAL(464.81533776455961515239323489367962),
            DECIMAL(522.19750545507190508942585438489914)}
    };

    // Step IV: Run It and Test!
    IterativeSphericalDistanceDeterminationAlgorithm algo(RADIUS_OF_EARTH,
                                                          std::move(cam),
                                                          DEFAULT_ITERATIONS_1,
                                                          DEFAULT_REFRESHES_0,
                                                          DEFAULT_DIST_TOL,
                                                          DEFAULT_DISC_RAT,
                                                          DEFAULT_PDF_ORD,
                                                          DEFAULT_R_L_ORD);

    PositionVector actual = algo.Run(pts);
    VECTOR_EQUALS(expected, actual, DEFAULT_TOLERANCE);
}
