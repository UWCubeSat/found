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
unsigned char imageDataNoEdgeAllSpace[25] = {0, 0, 0, 0, 0,
                                             0, 0, 0, 0, 0,
                                             0, 0, 0, 0, 0,
                                             0, 0, 0, 0, 0,
                                             0, 0, 0, 0, 0};
Image imageNoEdgeAllSpace = {5, 5, 1, imageDataNoEdgeAllSpace};
Points expectedNoEdgeAllSpace;
EdgeTestData testNoEdgeAllSpace = {&imageNoEdgeAllSpace, &expectedNoEdgeAllSpace};

// TestNoEdgeAllEarth
unsigned char imageDataNoEdgeAllEarth[25] = {5, 5, 5, 5, 5,
                                             5, 5, 5, 5, 5,
                                             5, 5, 5, 5, 5,
                                             5, 5, 5, 5, 5,
                                             5, 5, 5, 5, 5};
Image imageNoEdgeAllEarth = {5, 5, 1, imageDataNoEdgeAllEarth};
Points expectedNoEdgeAllEarth;
EdgeTestData testNoEdgeAllEarth = {&imageNoEdgeAllEarth, &expectedNoEdgeAllEarth};

// TestNoEdgeAllSpaceWithNoise
unsigned char imageDataNoEdgeAllSpaceWithNoise[25] = {5, 5, 5, 5, 5,
                                                     5, 0, 0, 0, 5,
                                                     5, 0, 0, 0, 5,
                                                     5, 0, 0, 0, 5,
                                                     5, 5, 5, 5, 5};
Image imageNoEdgeAllSpaceWithNoise = {5, 5, 1, imageDataNoEdgeAllSpaceWithNoise};
Points expectedNoEdgeAllSpaceWithNoise;
EdgeTestData testNoEdgeAllSpaceWithNoise = {&imageNoEdgeAllSpaceWithNoise, &expectedNoEdgeAllSpaceWithNoise};

// TestLeftStraightEdge
unsigned char imageDataLeftStraightEdge[25] = {5, 5, 5, 0, 0,
                                              5, 5, 5, 0, 0,
                                              5, 5, 5, 0, 0,
                                              5, 5, 5, 0, 0,
                                              5, 5, 5, 0, 0};
Image imageLeftStraightEdge = {5, 5, 1, imageDataLeftStraightEdge};
Points expectedLeftStraightEdge = {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}};
EdgeTestData testLeftStraightEdge = {&imageLeftStraightEdge, &expectedLeftStraightEdge};

// TestRightStraightEdge
unsigned char imageDataRightStraightEdge[25] = {0, 0, 5, 5, 5,
                                               0, 0, 5, 5, 5,
                                               0, 0, 5, 5, 5,
                                               0, 0, 5, 5, 5,
                                               0, 0, 5, 5, 5};
Image imageRightStraightEdge = {5, 5, 1, imageDataRightStraightEdge};
Points expectedRightStraightEdge = {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}};
EdgeTestData testRightStraightEdge = {&imageRightStraightEdge, &expectedRightStraightEdge};

// TestUpStraightEdge
unsigned char imageDataUpStraightEdge[25] = {5, 5, 5, 5, 5,
                                            5, 5, 5, 5, 5,
                                            5, 5, 5, 5, 5,
                                            0, 0, 0, 0, 0,
                                            0, 0, 0, 0, 0};
Image imageUpStraightEdge = {5, 5, 1, imageDataUpStraightEdge};
Points expectedUpStraightEdge = {{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}};
EdgeTestData testUpStraightEdge = {&imageUpStraightEdge, &expectedUpStraightEdge};

// TestDownStraightEdge
unsigned char imageDataDownStraightEdge[25] = {0, 0, 0, 0, 0,
                                              0, 0, 0, 0, 0,
                                              5, 5, 5, 5, 5,
                                              5, 5, 5, 5, 5,
                                              5, 5, 5, 5, 5};
Image imageDownStraightEdge = {5, 5, 1, imageDataDownStraightEdge};
Points expectedDownStraightEdge = {{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}};
EdgeTestData testDownStraightEdge = {&imageDownStraightEdge, &expectedDownStraightEdge};

// TestTopTriangleEdge
unsigned char imageDataTopTriangleEdge[25] = {0, 0, 0, 0, 0,
                                             0, 0, 0, 0, 0,
                                             0, 0, 5, 0, 0,
                                             0, 5, 5, 5, 0,
                                             5, 5, 5, 5, 5};
Image imageTopTriangleEdge = {5, 5, 1, imageDataTopTriangleEdge};
Points expectedTopTriangleEdge = {{0, 4}, {1, 3}, {2, 2}, {3, 3}, {4, 4}};
EdgeTestData testTopTriangleEdge = {&imageTopTriangleEdge, &expectedTopTriangleEdge};

// TestCornerQuarterEdge1
unsigned char imageDataCornerQuarterEdge1[25] = {5, 5, 5, 5, 5,
                                                0, 5, 5, 5, 5,
                                                0, 0, 5, 5, 5,
                                                0, 0, 0, 5, 5,
                                                0, 0, 0, 0, 5};
Image imageCornerQuarterEdge1 = {5, 5, 1, imageDataCornerQuarterEdge1};
Points expectedCornerQuarterEdge1_1 = {{0, 0}, {1, 1}, {2, 2}, {3, 3}};
Points expectedCornerQuarterEdge1_2 = {{4, 4}, {1, 1}, {2, 2}, {3, 3}};
// For this test, you may want to use both expectedCornerQuarterEdge1_1 and _2
EdgeTestData testCornerQuarterEdge1_1 = {&imageCornerQuarterEdge1, &expectedCornerQuarterEdge1_1};
EdgeTestData testCornerQuarterEdge1_2 = {&imageCornerQuarterEdge1, &expectedCornerQuarterEdge1_2};

// TestCornerQuarterEdge2
unsigned char imageDataCornerQuarterEdge2[20] = {5, 5, 5, 5, 0,
                                                5, 5, 5, 5, 0,
                                                5, 5, 0, 0, 0,
                                                0, 0, 0, 0, 0};
Image imageCornerQuarterEdge2 = {5, 4, 1, imageDataCornerQuarterEdge2};
Points expectedCornerQuarterEdge2 = {{3, 0}, {3, 1}, {1, 2}};
EdgeTestData testCornerQuarterEdge2 = {&imageCornerQuarterEdge2, &expectedCornerQuarterEdge2};

// TestSpaceBarelyVisible
unsigned char imageDataSpaceBarelyVisible[25] = {5, 5, 5, 5, 5,
                                                5, 5, 5, 5, 5,
                                                5, 5, 5, 5, 5,
                                                5, 5, 5, 5, 5,
                                                5, 5, 5, 5, 0};
Image imageSpaceBarelyVisible = {5, 5, 1, imageDataSpaceBarelyVisible};
Points expectedSpaceBarelyVisible = {{3, 4}, {4, 3}};
EdgeTestData testSpaceBarelyVisible = {&imageSpaceBarelyVisible, &expectedSpaceBarelyVisible};

// TestPlanetBarelyVisible
unsigned char imageDataPlanetBarelyVisible[25] = {5, 5, 0, 0, 0,
                                                 0, 0, 0, 0, 0,
                                                 0, 0, 0, 0, 0,
                                                 0, 0, 0, 0, 0,
                                                 0, 0, 0, 0, 0};
Image imagePlanetBarelyVisible = {5, 5, 1, imageDataPlanetBarelyVisible};
Points expectedPlanetBarelyVisible = {{0, 0}, {1, 0}};
EdgeTestData testPlanetBarelyVisible = {&imagePlanetBarelyVisible, &expectedPlanetBarelyVisible};

// TestNoiseInPlanet1
unsigned char imageDataNoiseInPlanet1[25] = {5, 5, 5, 0, 0,
                                            0, 0, 5, 0, 0,
                                            5, 5, 5, 0, 0,
                                            5, 0, 5, 0, 0,
                                            5, 0, 5, 0, 0};
Image imageNoiseInPlanet1 = {5, 5, 1, imageDataNoiseInPlanet1};
Points expectedNoiseInPlanet1 = {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}};
EdgeTestData testNoiseInPlanet1 = {&imageNoiseInPlanet1, &expectedNoiseInPlanet1};

// TestNoiseInPlanet2
unsigned char imageDataNoiseInPlanet2[25] = {5, 0, 5, 0, 0,
                                            5, 0, 5, 0, 0,
                                            5, 5, 5, 0, 0,
                                            0, 5, 5, 0, 0,
                                            5, 0, 5, 0, 0};
Image imageNoiseInPlanet2 = {5, 5, 1, imageDataNoiseInPlanet2};
Points expectedNoiseInPlanet2 = {{2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}};
EdgeTestData testNoiseInPlanet2 = {&imageNoiseInPlanet2, &expectedNoiseInPlanet2};

// TestNoiseInSpace
unsigned char imageDataNoiseInSpace[25] = {0, 0, 0, 0, 2,
                                          0, 9, 0, 7, 0,
                                          0, 0, 0, 0, 0,
                                          5, 5, 5, 5, 5,
                                          5, 5, 5, 5, 5};
Image imageNoiseInSpace = {5, 5, 1, imageDataNoiseInSpace};
Points expectedNoiseInSpace = {{0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}};
EdgeTestData testNoiseInSpace = {&imageNoiseInSpace, &expectedNoiseInSpace};



}

#endif  // TEST_COMMON_CONSTANTS_EDGE_CONSTANTS_HPP_
