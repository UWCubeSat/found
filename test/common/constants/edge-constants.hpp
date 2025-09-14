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
extern unsigned char imageDataNoEdgeAllSpace[25] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0
};
extern Image imageNoEdgeAllSpace = {5, 5, 1, imageDataNoEdgeAllSpace};
extern Points expectedNoEdgeAllSpace;
extern EdgeTestData testNoEdgeAllSpace = {&imageNoEdgeAllSpace, &expectedNoEdgeAllSpace};

// TestNoEdgeAllEarth
extern unsigned char imageDataNoEdgeAllEarth[25] = {
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5
};
extern Image imageNoEdgeAllEarth = {5, 5, 1, imageDataNoEdgeAllEarth};
extern Points expectedNoEdgeAllEarth;
extern EdgeTestData testNoEdgeAllEarth = {&imageNoEdgeAllEarth, &expectedNoEdgeAllEarth};

// TestNoEdgeAllSpaceWithNoise
extern unsigned char imageDataNoEdgeAllSpaceWithNoise[25] = {
    5, 5, 5, 5, 5,
    5, 0, 0, 0, 5,
    5, 0, 0, 0, 5,
    5, 0, 0, 0, 5,
    5, 5, 5, 5, 5
};
extern Image imageNoEdgeAllSpaceWithNoise = {5, 5, 1, imageDataNoEdgeAllSpaceWithNoise};
extern Points expectedNoEdgeAllSpaceWithNoise;
extern EdgeTestData testNoEdgeAllSpaceWithNoise = {&imageNoEdgeAllSpaceWithNoise, &expectedNoEdgeAllSpaceWithNoise};

// TestLeftStraightEdge
extern unsigned char imageDataLeftStraightEdge[25] = {
    5, 5, 5, 0, 0,
    5, 5, 5, 0, 0,
    5, 5, 5, 0, 0,
    5, 5, 5, 0, 0,
    5, 5, 5, 0, 0
};
extern Image imageLeftStraightEdge = {5, 5, 1, imageDataLeftStraightEdge};
extern Points expectedLeftStraightEdge = {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}};
extern EdgeTestData testLeftStraightEdge = {&imageLeftStraightEdge, &expectedLeftStraightEdge};

// TestRightStraightEdge
extern unsigned char imageDataRightStraightEdge[25] = {
    0, 0, 5, 5, 5,
    0, 0, 5, 5, 5,
    0, 0, 5, 5, 5,
    0, 0, 5, 5, 5,
    0, 0, 5, 5, 5
};
extern Image imageRightStraightEdge = {5, 5, 1, imageDataRightStraightEdge};
extern Points expectedRightStraightEdge = {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}};
extern EdgeTestData testRightStraightEdge = {&imageRightStraightEdge, &expectedRightStraightEdge};

// TestUpStraightEdge
extern unsigned char imageDataUpStraightEdge[25] = {
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0
};
extern Image imageUpStraightEdge = {5, 5, 1, imageDataUpStraightEdge};
extern Points expectedUpStraightEdge = {{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}};
extern EdgeTestData testUpStraightEdge = {&imageUpStraightEdge, &expectedUpStraightEdge};

// TestDownStraightEdge
extern unsigned char imageDataDownStraightEdge[25] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5
};
extern Image imageDownStraightEdge = {5, 5, 1, imageDataDownStraightEdge};
extern Points expectedDownStraightEdge = {{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}};
extern EdgeTestData testDownStraightEdge = {&imageDownStraightEdge, &expectedDownStraightEdge};

// TestTopTriangleEdge
extern unsigned char imageDataTopTriangleEdge[25] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 5, 0, 0,
    0, 5, 5, 5, 0,
    5, 5, 5, 5, 5
};
extern Image imageTopTriangleEdge = {5, 5, 1, imageDataTopTriangleEdge};
extern Points expectedTopTriangleEdge = {{0, 4}, {1, 3}, {2, 2}, {3, 3}, {4, 4}};
extern EdgeTestData testTopTriangleEdge = {&imageTopTriangleEdge, &expectedTopTriangleEdge};

// TestCornerQuarterEdge1
extern unsigned char imageDataCornerQuarterEdge1[25] = {
    5, 5, 5, 5, 5,
    0, 5, 5, 5, 5,
    0, 0, 5, 5, 5,
    0, 0, 0, 5, 5,
    0, 0, 0, 0, 5
};
extern Image imageCornerQuarterEdge1 = {5, 5, 1, imageDataCornerQuarterEdge1};
extern Points expectedCornerQuarterEdge1_1 = {{0, 0}, {1, 1}, {2, 2}, {3, 3}};
extern Points expectedCornerQuarterEdge1_2 = {{4, 4}, {1, 1}, {2, 2}, {3, 3}};
// For this test, you may want to use both expectedCornerQuarterEdge1_1 and _2
extern EdgeTestData testCornerQuarterEdge1_1 = {&imageCornerQuarterEdge1, &expectedCornerQuarterEdge1_1};
extern EdgeTestData testCornerQuarterEdge1_2 = {&imageCornerQuarterEdge1, &expectedCornerQuarterEdge1_2};

// TestCornerQuarterEdge2
extern unsigned char imageDataCornerQuarterEdge2[20] = {
    5, 5, 5, 5, 0,
    5, 5, 5, 5, 0,
    5, 5, 0, 0, 0,
    0, 0, 0, 0, 0
};
extern Image imageCornerQuarterEdge2 = {5, 4, 1, imageDataCornerQuarterEdge2};
extern Points expectedCornerQuarterEdge2 = {{3, 0}, {3, 1}, {1, 2}};
extern EdgeTestData testCornerQuarterEdge2 = {&imageCornerQuarterEdge2, &expectedCornerQuarterEdge2};

// TestSpaceBarelyVisible
extern unsigned char imageDataSpaceBarelyVisible[25] = {
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 0
};
extern Image imageSpaceBarelyVisible = {5, 5, 1, imageDataSpaceBarelyVisible};
extern Points expectedSpaceBarelyVisible = {{3, 4}, {4, 3}};
extern EdgeTestData testSpaceBarelyVisible = {&imageSpaceBarelyVisible, &expectedSpaceBarelyVisible};

// TestPlanetBarelyVisible
extern unsigned char imageDataPlanetBarelyVisible[25] = {
    5, 5, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0
};
extern Image imagePlanetBarelyVisible = {5, 5, 1, imageDataPlanetBarelyVisible};
extern Points expectedPlanetBarelyVisible = {{0, 0}, {1, 0}};
extern EdgeTestData testPlanetBarelyVisible = {&imagePlanetBarelyVisible, &expectedPlanetBarelyVisible};

// TestNoiseInPlanet1
extern unsigned char imageDataNoiseInPlanet1[25] = {
    5, 5, 5, 0, 0,
    0, 0, 5, 0, 0,
    5, 5, 5, 0, 0,
    5, 0, 5, 0, 0,
    5, 0, 5, 0, 0
};
extern Image imageNoiseInPlanet1 = {5, 5, 1, imageDataNoiseInPlanet1};
extern Points expectedNoiseInPlanet1 = {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}};
extern EdgeTestData testNoiseInPlanet1 = {&imageNoiseInPlanet1, &expectedNoiseInPlanet1};

// TestNoiseInPlanet2
extern unsigned char imageDataNoiseInPlanet2[25] = {
    5, 0, 5, 0, 0,
    5, 0, 5, 0, 0,
    5, 5, 5, 0, 0,
    0, 5, 5, 0, 0,
    5, 0, 5, 0, 0
};
extern Image imageNoiseInPlanet2 = {5, 5, 1, imageDataNoiseInPlanet2};
extern Points expectedNoiseInPlanet2 = {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}};
extern EdgeTestData testNoiseInPlanet2 = {&imageNoiseInPlanet2, &expectedNoiseInPlanet2};

// TestNoiseInSpace
extern unsigned char imageDataNoiseInSpace[25] = {
    0, 0, 0, 0, 2,
    0, 9, 0, 7, 0,
    0, 0, 0, 0, 0,
    5, 5, 5, 5, 5,
    5, 5, 5, 5, 5
};
extern Image imageNoiseInSpace = {5, 5, 1, imageDataNoiseInSpace};
extern Points expectedNoiseInSpace = {{0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}};
extern EdgeTestData testNoiseInSpace = {&imageNoiseInSpace, &expectedNoiseInSpace};



}

#endif  // TEST_COMMON_CONSTANTS_EDGE_CONSTANTS_HPP_
