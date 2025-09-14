#ifndef TEST_COMMON_CONSTANTS_EDGE_CONSTANTS_HPP_
#define TEST_COMMON_CONSTANTS_EDGE_CONSTANTS_HPP_

#include "src/common/decimal.hpp"

#include "src/distance/edge.hpp"

namespace found {

struct EdgeTestData {
    Image *image;
    Points *expectedPoints;

};


// TestNoEdgeAllSpace
inline unsigned char imageDataNoEdgeAllSpace[25] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0
};
inline Image imageNoEdgeAllSpace = {5, 5, 1, imageDataNoEdgeAllSpace};
inline Points expectedNoEdgeAllSpace;
inline EdgeTestData testNoEdgeAllSpace = {&imageNoEdgeAllSpace, &expectedNoEdgeAllSpace};

// TestNoEdgeAllEarth
inline unsigned char imageDataNoEdgeAllEarth[25] = {
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5
};
inline Image imageNoEdgeAllEarth = {5, 5, 1, imageDataNoEdgeAllEarth};
inline Points expectedNoEdgeAllEarth;
inline EdgeTestData testNoEdgeAllEarth = {&imageNoEdgeAllEarth, &expectedNoEdgeAllEarth};

// TestNoEdgeAllSpaceWithNoise
inline unsigned char imageDataNoEdgeAllSpaceWithNoise[25] = {
    5, 5, 5, 5, 5,
    5, 0, 0, 0, 5,
    5, 0, 0, 0, 5,
    5, 0, 0, 0, 5,
    5, 5, 5, 5, 5
};
inline Image imageNoEdgeAllSpaceWithNoise = {5, 5, 1, imageDataNoEdgeAllSpaceWithNoise};
inline Points expectedNoEdgeAllSpaceWithNoise;
inline EdgeTestData testNoEdgeAllSpaceWithNoise = {&imageNoEdgeAllSpaceWithNoise, &expectedNoEdgeAllSpaceWithNoise};

// TestLeftStraightEdge
inline unsigned char imageDataLeftStraightEdge[25] = {
    5, 5, 5, 0, 0,
    5, 5, 5, 0, 0,
    5, 5, 5, 0, 0,
    5, 5, 5, 0, 0,
    5, 5, 5, 0, 0
};
inline Image imageLeftStraightEdge = {5, 5, 1, imageDataLeftStraightEdge};
inline Points expectedLeftStraightEdge = {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}};
inline EdgeTestData testLeftStraightEdge = {&imageLeftStraightEdge, &expectedLeftStraightEdge};

// TestRightStraightEdge
inline unsigned char imageDataRightStraightEdge[25] = {
    0, 0, 5, 5, 5,
    0, 0, 5, 5, 5,
    0, 0, 5, 5, 5,
    0, 0, 5, 5, 5,
    0, 0, 5, 5, 5
};
inline Image imageRightStraightEdge = {5, 5, 1, imageDataRightStraightEdge};
inline Points expectedRightStraightEdge = {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}};
inline EdgeTestData testRightStraightEdge = {&imageRightStraightEdge, &expectedRightStraightEdge};

// TestUpStraightEdge
inline unsigned char imageDataUpStraightEdge[25] = {
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0
};
inline Image imageUpStraightEdge = {5, 5, 1, imageDataUpStraightEdge};
inline Points expectedUpStraightEdge = {{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}};
inline EdgeTestData testUpStraightEdge = {&imageUpStraightEdge, &expectedUpStraightEdge};

// TestDownStraightEdge
inline unsigned char imageDataDownStraightEdge[25] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5
};
inline Image imageDownStraightEdge = {5, 5, 1, imageDataDownStraightEdge};
inline Points expectedDownStraightEdge = {{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}};
inline EdgeTestData testDownStraightEdge = {&imageDownStraightEdge, &expectedDownStraightEdge};

// TestTopTriangleEdge
inline unsigned char imageDataTopTriangleEdge[25] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 5, 0, 0,
    0, 5, 5, 5, 0,
    5, 5, 5, 5, 5
};
inline Image imageTopTriangleEdge = {5, 5, 1, imageDataTopTriangleEdge};
inline Points expectedTopTriangleEdge = {{0, 4}, {1, 3}, {2, 2}, {3, 3}, {4, 4}};
inline EdgeTestData testTopTriangleEdge = {&imageTopTriangleEdge, &expectedTopTriangleEdge};

// TestCornerQuarterEdge1
inline unsigned char imageDataCornerQuarterEdge1[25] = {
    5, 5, 5, 5, 5,
    0, 5, 5, 5, 5,
    0, 0, 5, 5, 5,
    0, 0, 0, 5, 5,
    0, 0, 0, 0, 5
};
inline Image imageCornerQuarterEdge1 = {5, 5, 1, imageDataCornerQuarterEdge1};
inline Points expectedCornerQuarterEdge1_1 = {{0, 0}, {1, 1}, {2, 2}, {3, 3}};
inline Points expectedCornerQuarterEdge1_2 = {{4, 4}, {1, 1}, {2, 2}, {3, 3}};
// For this test, you may want to use both expectedCornerQuarterEdge1_1 and _2
inline EdgeTestData testCornerQuarterEdge1_1 = {&imageCornerQuarterEdge1, &expectedCornerQuarterEdge1_1};
inline EdgeTestData testCornerQuarterEdge1_2 = {&imageCornerQuarterEdge1, &expectedCornerQuarterEdge1_2};

// TestCornerQuarterEdge2
inline unsigned char imageDataCornerQuarterEdge2[20] = {
    5, 5, 5, 5, 0,
    5, 5, 5, 5, 0,
    5, 5, 0, 0, 0,
    0, 0, 0, 0, 0
};
inline Image imageCornerQuarterEdge2 = {5, 4, 1, imageDataCornerQuarterEdge2};
inline Points expectedCornerQuarterEdge2 = {{3, 0}, {3, 1}, {1, 2}};
inline EdgeTestData testCornerQuarterEdge2 = {&imageCornerQuarterEdge2, &expectedCornerQuarterEdge2};

// TestSpaceBarelyVisible
inline unsigned char imageDataSpaceBarelyVisible[25] = {
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 0
};
inline Image imageSpaceBarelyVisible = {5, 5, 1, imageDataSpaceBarelyVisible};
inline Points expectedSpaceBarelyVisible = {{3, 4}, {4, 3}};
inline EdgeTestData testSpaceBarelyVisible = {&imageSpaceBarelyVisible, &expectedSpaceBarelyVisible};

// TestPlanetBarelyVisible
inline unsigned char imageDataPlanetBarelyVisible[25] = {
    5, 5, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0
};
inline Image imagePlanetBarelyVisible = {5, 5, 1, imageDataPlanetBarelyVisible};
inline Points expectedPlanetBarelyVisible = {{0, 0}, {1, 0}};
inline EdgeTestData testPlanetBarelyVisible = {&imagePlanetBarelyVisible, &expectedPlanetBarelyVisible};

// TestNoiseInPlanet1
inline unsigned char imageDataNoiseInPlanet1[25] = {
    5, 5, 5, 0, 0,
    0, 0, 5, 0, 0,
    5, 5, 5, 0, 0,
    5, 0, 5, 0, 0,
    5, 0, 5, 0, 0
};
inline Image imageNoiseInPlanet1 = {5, 5, 1, imageDataNoiseInPlanet1};
inline Points expectedNoiseInPlanet1 = {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}};
inline EdgeTestData testNoiseInPlanet1 = {&imageNoiseInPlanet1, &expectedNoiseInPlanet1};

// TestNoiseInPlanet2
inline unsigned char imageDataNoiseInPlanet2[25] = {
    5, 0, 5, 0, 0,
    5, 0, 5, 0, 0,
    5, 5, 5, 0, 0,
    0, 5, 5, 0, 0,
    5, 0, 5, 0, 0
};
inline Image imageNoiseInPlanet2 = {5, 5, 1, imageDataNoiseInPlanet2};
inline Points expectedNoiseInPlanet2 = {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}};
inline EdgeTestData testNoiseInPlanet2 = {&imageNoiseInPlanet2, &expectedNoiseInPlanet2};

// TestNoiseInSpace
inline unsigned char imageDataNoiseInSpace[25] = {
    0, 0, 0, 0, 2,
    0, 9, 0, 7, 0,
    0, 0, 0, 0, 0,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5
};
inline Image imageNoiseInSpace = {5, 5, 1, imageDataNoiseInSpace};
inline Points expectedNoiseInSpace = {{0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}};
inline EdgeTestData testNoiseInSpace = {&imageNoiseInSpace, &expectedNoiseInSpace};



}

#endif  // TEST_COMMON_CONSTANTS_EDGE_CONSTANTS_HPP_
