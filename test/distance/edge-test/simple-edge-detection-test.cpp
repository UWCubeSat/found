#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <algorithm>
#include <vector>

#include "test/common/common.hpp"
#include "test/common/constants/edge-constants.hpp"

#include "src/common/style.hpp"

#include "src/distance/edge.hpp"

namespace found {

MATCHER_P(Vec2Equal, expected, "") {
    return abs(expected.x - arg.x) < DEFAULT_TOLERANCE &&
           abs(expected.y - arg.y) < DEFAULT_TOLERANCE;
}

TEST(SimpleEdgeDetectionTest, TestNoEdgeAllSpace) {
    unsigned char imageData[25] = {0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0};
    Image image = {
        5,
        5,
        1,
        imageData
    };

    Points expected;
    Points actual = minimalSEDA.Run(image);

    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Vec2& val) {
                    return Vec2Equal(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}


TEST(SimpleEdgeDetectionTest, TestNoEdgeAllEarth) {
    unsigned char imageData[25] = {5, 5, 5, 5, 5,
                                   5, 5, 5, 5, 5,
                                   5, 5, 5, 5, 5,
                                   5, 5, 5, 5, 5,
                                   5, 5, 5, 5, 5};
    Image image = {
        5,
        5,
        1,
        imageData
    };

    Points expected;
    Points actual = minimalSEDA.Run(image);

    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Vec2& val) {
                    return Vec2Equal(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(SimpleEdgeDetectionTest, TestNoEdgeAllSpaceWithNoise) {
    unsigned char imageData[25] = {5, 5, 5, 5, 5,
                                   5, 0, 0, 0, 5,
                                   5, 0, 0, 0, 5,
                                   5, 0, 0, 0, 5,
                                   5, 5, 5, 5, 5};
    Image image = {
        5,
        5,
        1,
        imageData
    };

    Points expected;
    Points actual = minimalSEDA.Run(image);

    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Vec2& val) {
                    return Vec2Equal(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(SimpleEdgeDetectionTest, TestLeftStraightEdge) {
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
        {2, 0},
        {2, 1},
        {2, 2},
        {2, 3},
        {2, 4}
    };
    Points actual = thickerBorderSEDA.Run(image);

    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Vec2& val) {
                    return Vec2Equal(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(SimpleEdgeDetectionTest, TestRightStraightEdge) {
    unsigned char imageData[25] = {0, 0, 5, 5, 5,
                                   0, 0, 5, 5, 5,
                                   0, 0, 5, 5, 5,
                                   0, 0, 5, 5, 5,
                                   0, 0, 5, 5, 5};
    Image image = {
        5,
        5,
        1,
        imageData
    };

    Points expected = {
        {2, 0},
        {2, 1},
        {2, 2},
        {2, 3},
        {2, 4}
    };
    Points actual = minimalSEDA.Run(image);

    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Vec2& val) {
                    return Vec2Equal(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(SimpleEdgeDetectionTest, TestUpStraightEdge) {
    unsigned char imageData[25] = {5, 5, 5, 5, 5,
                                   5, 5, 5, 5, 5,
                                   5, 5, 5, 5, 5,
                                   0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0};
    Image image = {
        5,
        5,
        1,
        imageData
    };

    Points expected = {
        {0, 2},
        {1, 2},
        {2, 2},
        {3, 2},
        {4, 2}
    };
    Points actual = minimalSEDA.Run(image);

    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Vec2& val) {
                    return Vec2Equal(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(SimpleEdgeDetectionTest, TestDownStraightEdge) {
    unsigned char imageData[25] = {0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0,
                                   5, 5, 5, 5, 5,
                                   5, 5, 5, 5, 5,
                                   5, 5, 5, 5, 5};
    Image image = {
        5,
        5,
        1,
        imageData
    };

    Points expected = {
        {0, 2},
        {1, 2},
        {2, 2},
        {3, 2},
        {4, 2}
    };
    Points actual = thickerBorderSEDA.Run(image);

    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Vec2& val) {
                    return Vec2Equal(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(SimpleEdgeDetectionTest, TestTopTriangleEdge) {
    unsigned char imageData[25] = {0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0,
                                   0, 0, 5, 0, 0,
                                   0, 5, 5, 5, 0,
                                   5, 5, 5, 5, 5};
    Image image = {
        5,
        5,
        1,
        imageData
    };

    Points expected = {
        {0, 4},
        {1, 3},
        {2, 2},
        {3, 3},
        {4, 4}
    };
    Points actual = minimalSEDA.Run(image);

    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Vec2& val) {
                    return Vec2Equal(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(SimpleEdgeDetectionTest, TestCornerQuarterEdge1) {
    unsigned char imageData[25] = {5, 5, 5, 5, 5,
                                   0, 5, 5, 5, 5,
                                   0, 0, 5, 5, 5,
                                   0, 0, 0, 5, 5,
                                   0, 0, 0, 0, 5};
    Image image = {
        5,
        5,
        1,
        imageData
    };

    Points expected1 = {
        {0, 0},
        {1, 1},
        {2, 2},
        {3, 3},
    };
    Points expected2 = {
        {4, 4},
        {1, 1},
        {2, 2},
        {3, 3},
    };
    Points actual = minimalSEDA.Run(image);

    ASSERT_TRUE(std::is_permutation(expected1.begin(),
                        expected1.end(),
                        actual.begin(),
                        actual.end(),
                        vectorEqual) ||
                std::is_permutation(expected2.begin(),
                        expected2.end(),
                        actual.begin(),
                        actual.end(),
                        vectorEqual));
}

TEST(SimpleEdgeDetectionTest, TestCornerQuarterEdge2) {
    unsigned char imageData[20] = {5, 5, 5, 5, 0,
                                   5, 5, 5, 5, 0,
                                   5, 5, 0, 0, 0,
                                   0, 0, 0, 0, 0};
    Image image = {
        5,
        4,
        1,
        imageData
    };

    Points expected = {
        {3, 0},
        {3, 1},
        {1, 2},
    };
    Points actual = minimalSEDA.Run(image);

    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Vec2& val) {
                    return Vec2Equal(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(SimpleEdgeDetectionTest, TestSpaceBarelyVisible) {
    unsigned char imageData[25] = {5, 5, 5, 5, 5,
                                   5, 5, 5, 5, 5,
                                   5, 5, 5, 5, 5,
                                   5, 5, 5, 5, 5,
                                   5, 5, 5, 5, 0};
    Image image = {
        5,
        5,
        1,
        imageData
    };

    // Here, it can be either one of two
    Points expected = {
        {3, 4},
        {4, 3}
    };
    Points actual = minimalSEDA.Run(image);

    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Vec2& val) {
                    return Vec2Equal(val);
                });

    ASSERT_EQ(static_cast<size_t>(1), actual.size());
    ASSERT_TRUE(vectorEqual(expected[0], actual[0]) || vectorEqual(expected[1], actual[0]));
}

TEST(SimpleEdgeDetectionTest, TestPlanetBarelyVisible) {
    unsigned char imageData[25] = {5, 5, 0, 0, 0,
                                   0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0};
    Image image = {
        5,
        5,
        1,
        imageData
    };

    Points expected = {
        {0, 0},
        {1, 0},
    };
    Points actual = thickerBorderSEDA.Run(image);

    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Vec2& val) {
                    return Vec2Equal(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(SimpleEdgeDetectionTest, TestNoiseInPlanet1) {
    unsigned char imageData[25] = {5, 5, 5, 0, 0,
                                   0, 0, 5, 0, 0,
                                   5, 5, 5, 0, 0,
                                   5, 0, 5, 0, 0,
                                   5, 0, 5, 0, 0};
    Image image = {
        5,
        5,
        1,
        imageData
    };

    Points expected = {
        {2, 0},
        {2, 1},
        {2, 2},
        {2, 3},
        {2, 4}
    };
    Points actual = minimalSEDA.Run(image);

    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Vec2& val) {
                    return Vec2Equal(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(SimpleEdgeDetectionTest, TestNoiseInPlanet2) {
    unsigned char imageData[25] = {5, 0, 5, 0, 0,
                                   5, 0, 5, 0, 0,
                                   5, 5, 5, 0, 0,
                                   0, 5, 5, 0, 0,
                                   5, 0, 5, 0, 0};
    Image image = {
        5,
        5,
        1,
        imageData
    };

    Points expected = {
        {2, 0},
        {2, 1},
        {2, 2},
        {2, 3},
        {2, 4}
    };
    Points actual = minimalSEDA.Run(image);

    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Vec2& val) {
                    return Vec2Equal(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(SimpleEdgeDetectionTest, TestNoiseInSpace) {
    unsigned char imageData[25] = {0, 0, 0, 0, 2,
                                   0, 9, 0, 7, 0,
                                   0, 0, 0, 0, 0,
                                   5, 5, 5, 5, 5,
                                   5, 5, 5, 5, 5};
    Image image = {
        5,
        5,
        1,
        imageData
    };

    Points expected = {
        {0, 3},
        {1, 3},
        {2, 3},
        {3, 3},
        {4, 3}
    };
    Points actual = thickerBorderSEDA.Run(image);

    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Vec2& val) {
                    return Vec2Equal(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
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
