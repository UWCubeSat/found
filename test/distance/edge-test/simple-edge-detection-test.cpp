#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <algorithm>
#include <vector>

#include "test/common/common.hpp"
#include "test/common/constants/edge-constants.hpp"

#include "src/common/style.hpp"

#include "src/distance/edge.hpp"

namespace found {

struct SimpleEdgeDetectionTestParams {
    const char* name;
    SimpleEdgeDetectionAlgorithm* algorithm;
    EdgeTestData* testData;
};

const decimal offset = 0.5;

SimpleEdgeDetectionAlgorithm minimalSEDA(5, 1, 0);
SimpleEdgeDetectionAlgorithm thickerBorderSEDA(5, 2, 0);
SimpleEdgeDetectionAlgorithm offsetSEDA(5, 1, offset);

MATCHER_P(Vec2Equal, expected, "") {
    return abs(expected.x - arg.x) < DEFAULT_TOLERANCE &&
           abs(expected.y - arg.y) < DEFAULT_TOLERANCE;
}

class SimpleEdgeDetectionParameterizedTestFixture : public ::testing::TestWithParam<SimpleEdgeDetectionTestParams> {};

TEST_P(SimpleEdgeDetectionParameterizedTestFixture, OutputMatchesExpected) {
    const auto& param = GetParam();
    Points actual = param.algorithm->Run(*param.testData->image);
    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(param.testData->expectedPoints->begin(), param.testData->expectedPoints->end(), std::back_inserter(matchers),
        [](const Vec2& val) { return Vec2Equal(val); });
    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

INSTANTIATE_TEST_SUITE_P(
    SimpleEdgeDetectionTest,
    SimpleEdgeDetectionParameterizedTestFixture,
    ::testing::Values(
        SimpleEdgeDetectionTestParams{"TestNoEdgeAllSpace", &minimalSEDA, &testNoEdgeAllSpace},
        SimpleEdgeDetectionTestParams{"TestNoEdgeAllEarth", &minimalSEDA, &testNoEdgeAllEarth},
        SimpleEdgeDetectionTestParams{"TestNoEdgeAllSpaceWithNoise", &minimalSEDA, &testNoEdgeAllSpaceWithNoise},
        SimpleEdgeDetectionTestParams{"TestLeftStraightEdge", &thickerBorderSEDA, &testLeftStraightEdge},
        SimpleEdgeDetectionTestParams{"TestRightStraightEdge", &minimalSEDA, &testRightStraightEdge},
        SimpleEdgeDetectionTestParams{"TestUpStraightEdge", &minimalSEDA, &testUpStraightEdge},
        SimpleEdgeDetectionTestParams{"TestDownStraightEdge", &thickerBorderSEDA, &testDownStraightEdge},
        SimpleEdgeDetectionTestParams{"TestTopTriangleEdge", &minimalSEDA, &testTopTriangleEdge},
        SimpleEdgeDetectionTestParams{"TestCornerQuarterEdge2", &minimalSEDA, &testCornerQuarterEdge2},
        SimpleEdgeDetectionTestParams{"TestPlanetBarelyVisible", &thickerBorderSEDA, &testPlanetBarelyVisible},
        SimpleEdgeDetectionTestParams{"TestNoiseInPlanet1", &minimalSEDA, &testNoiseInPlanet1},
        SimpleEdgeDetectionTestParams{"TestNoiseInPlanet2", &minimalSEDA, &testNoiseInPlanet2},
        SimpleEdgeDetectionTestParams{"TestNoiseInSpace", &thickerBorderSEDA, &testNoiseInSpace},
        SimpleEdgeDetectionTestParams{"TestSpaceBarelyVisible", &minimalSEDA, &testSpaceBarelyVisible}
    )
);

TEST(SimpleEdgeDetectionTest, TestCornerQuarterEdge1) {
    Points actual = minimalSEDA.Run(*testCornerQuarterEdge1_1.image);
    ASSERT_TRUE(std::is_permutation(testCornerQuarterEdge1_1.expectedPoints->begin(),
            testCornerQuarterEdge1_1.expectedPoints->end(),
            actual.begin(),
            actual.end(),
            vectorEqual) ||
        std::is_permutation(testCornerQuarterEdge1_2.expectedPoints->begin(),
            testCornerQuarterEdge1_2.expectedPoints->end(),
            actual.begin(),
            actual.end(),
            vectorEqual));
}

TEST(SimpleEdgeDetectionTest, TestSpaceBarelyVisible) {
    Points actual = minimalSEDA.Run(*testSpaceBarelyVisible.image);
    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(testSpaceBarelyVisible.expectedPoints->begin(), testSpaceBarelyVisible.expectedPoints->end(), std::back_inserter(matchers),
                [](const Vec2& val) { return Vec2Equal(val); });
    ASSERT_EQ(static_cast<size_t>(1), actual.size());
    ASSERT_TRUE(vectorEqual((*testSpaceBarelyVisible.expectedPoints)[0], actual[0]) || vectorEqual((*testSpaceBarelyVisible.expectedPoints)[1], actual[0]));
}

TEST(SimpleEdgeDetectionTest, TestHorizontalOffset) {
    unsigned char imageData[25] = {5, 5, 5, 0, 0,
                                   5, 5, 5, 0, 0,
                                   5, 5, 5, 0, 0,
                                   5, 5, 5, 0, 0,
                                   5, 5, 5, 0, 0};
    Image image = {
        5,
        5,
        1,
        imageData
    };

    Points expected = {
        {2 - offset, 0},
        {2 - offset, 1},
        {2 - offset, 2},
        {2 - offset, 3},
        {2 - offset, 4}
    };
    Points actual = offsetSEDA.Run(image);

    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Vec2& val) {
                    return Vec2Equal(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(SimpleEdgeDetectionTest, TestVerticalOffset) {
    unsigned char imageData[25] = {0, 0, 0, 0, 0,
                                   0, 5, 0, 0, 0,
                                   5, 5, 5, 5, 0,
                                   5, 5, 5, 5, 0,
                                   5, 5, 5, 5, 5};
    Image image = {
        5,
        5,
        1,
        imageData
    };

    Points expected = {
        {0, 2 + offset},
        {1, 1 + offset},
        {2, 2 + offset},
        {3, 2 + offset},
        {4, 4 + offset}
    };
    Points actual = offsetSEDA.Run(image);

    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Vec2& val) {
                    return Vec2Equal(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

}  // namespace found
