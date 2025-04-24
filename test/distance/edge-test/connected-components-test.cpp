#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cmath>
#include <vector>
#include <algorithm>

#include "test/common/common.hpp"

#include "src/style/style.hpp"

#include "src/distance/edge.hpp"

namespace found {

constexpr auto criteria = [](uint64_t index, Image &image) {
    return image.image[index] > 0;
};

constexpr auto vectorEqual = [](const Vec2 &a, const Vec2 &b) {
    return abs(a.x - b.x) < DEFAULT_TOLERANCE && abs(a.y - b.y) < DEFAULT_TOLERANCE;
};

MATCHER_P(EdgeEqual, expected, "") {
    return std::is_permutation(expected.points.begin(),
                               expected.points.end(),
                               arg.points.begin(),
                               arg.points.end(),
                               vectorEqual) &&
           vectorEqual(arg.upperLeft, expected.upperLeft) &&
           vectorEqual(arg.lowerRight, expected.lowerRight);
}

TEST(ConnectedComponentsTest, TestOnePixelBase) {
    // Setup Dependencies
    unsigned char imageData[4] = {1, 0,
                                  0, 0};

    Image image = {
        2,
        2,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {0, 0}
            },
            {0, 0},
            {0, 0}
        }
    };


    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, TestOnePixelCorner) {
    // Setup Dependencies
    unsigned char imageData[4] = {0, 1,
                                  0, 0};

    Image image = {
        2,
        2,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {1, 0}
            },
            {1, 0},
            {1, 0}
        }
    };


    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, TestTwoPixels) {
    // Setup Dependencies
    unsigned char imageData[4] = {1, 1,
                                  0, 0};

    Image image = {
        2,
        2,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {0, 0},
                {1, 0}
            },
            {0, 0},
            {1, 0}
        }
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, TestTwoPixelsDiagonalNormal) {
    // Setup Dependencies
    unsigned char imageData[4] = {1, 0,
                                  0, 1};

    Image image = {
        2,
        2,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {0, 0},
                {1, 1}
            },
            {0, 0},
            {1, 1}
        },
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, TestTwoPixelsDiagonalReverse) {
    // Setup Dependencies
    unsigned char imageData[4] = {0, 1,
                                  1, 0};

    Image image = {
        2,
        2,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {1, 0},
                {0, 1}
            },
            {0, 0},
            {1, 1}
        },
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, TestDoubleDiagonal) {
    // Setup Dependencies
    unsigned char imageData[9] = {1, 0, 1,
                                  0, 1, 0,
                                  1, 0, 1};

    Image image = {
        3,
        3,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {0, 0},
                {2, 0},
                {1, 1},
                {0, 2},
                {2, 2}
            },
            {0, 0},
            {2, 2}
        },
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, TestLineDiagonalVertical) {
    // Setup Dependencies
    unsigned char imageData[9] = {1, 0, 1,
                                  1, 1, 0,
                                  1, 0, 0};

    Image image = {
        3,
        3,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {0, 0},
                {2, 0},
                {1, 1},
                {0, 2},
                {0, 1}
            },
            {0, 0},
            {2, 2}
        },
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, TestLineDiagonalHorizontal1) {
    // Setup Dependencies
    unsigned char imageData[9] = {1, 1, 1,
                                  0, 1, 0,
                                  0, 0, 1};

    Image image = {
        3,
        3,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {0, 0},
                {1, 0},
                {2, 0},
                {1, 1},
                {2, 2},
            },
            {0, 0},
            {2, 2}
        },
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, TestLineDiagonalHorizontal2) {
    // Setup Dependencies
    unsigned char imageData[9] = {1, 0, 0,
                                  0, 1, 0,
                                  1, 1, 1};

    Image image = {
        3,
        3,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {0, 0},
                {1, 1},
                {0, 2},
                {1, 2},
                {2, 2},
            },
            {0, 0},
            {2, 2}
        },
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, Test2ConvergingLines1) {
    // Setup Dependencies
    unsigned char imageData[25] = {0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 1,
                                   1, 1, 1, 1, 0,
                                   0, 0, 1, 1, 1,
                                   0, 0, 0, 1, 1};

    Image image = {
        5,
        5,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {4, 1},
                {0, 2},
                {1, 2},
                {2, 2},
                {3, 2},
                {2, 3},
                {3, 3},
                {4, 3},
                {3, 4},
                {4, 4},
            },
            {0, 1},
            {4, 4}
        },
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(expected.size(), actual.size());

    ASSERT_EQ(expected[0].points.size(), actual[0].points.size());

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, Test2ConvergingLines2) {
    // Setup Dependencies
    unsigned char imageData[25] = {0, 0, 0, 0, 1,
                                   1, 0, 1, 0, 1,
                                   1, 0, 1, 1, 0,
                                   1, 1, 1, 1, 0,
                                   0, 0, 0, 0, 0};

    Image image = {
        5,
        5,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {4, 0},
                {0, 1},
                {2, 1},
                {4, 1},
                {0, 2},
                {2, 2},
                {3, 2},
                {0, 3},
                {1, 3},
                {2, 3},
                {3, 3}
            },
            {0, 0},
            {4, 3}
        },
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(expected.size(), actual.size());

    ASSERT_EQ(expected[0].points.size(), actual[0].points.size());

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, Test2ConvergingLines3) {
    // Setup Dependencies
    unsigned char imageData[25] = {0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 1,
                                   1, 1, 1, 1, 0,
                                   1, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0};

    Image image = {
        5,
        5,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {4, 1},
                {0, 2},
                {1, 2},
                {2, 2},
                {3, 2},
                {0, 3}
            },
            {0, 1},
            {4, 3}
        },
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(expected.size(), actual.size());

    ASSERT_EQ(expected[0].points.size(), actual[0].points.size());

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, Test3ConvergingLines1) {
    // Setup Dependencies
    unsigned char imageData[25] = {1, 0, 1, 0, 1,
                                   1, 0, 1, 0, 1,
                                   1, 0, 1, 0, 1,
                                   1, 0, 1, 0, 1,
                                   1, 1, 1, 1, 1};

    Image image = {
        5,
        5,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {0, 0},
                {2, 0},
                {4, 0},
                {0, 1},
                {2, 1},
                {4, 1},
                {0, 2},
                {2, 2},
                {4, 2},
                {0, 3},
                {2, 3},
                {4, 3},
                {0, 4},
                {1, 4},
                {2, 4},
                {3, 4},
                {4, 4}
            },
            {0, 0},
            {4, 4}
        },
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(expected.size(), actual.size());

    ASSERT_EQ(expected[0].points.size(), actual[0].points.size());

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, Test3ConvergingLines2) {
    // Setup Dependencies
    unsigned char imageData[25] = {0, 0, 0, 0, 1,
                                   0, 0, 1, 0, 1,
                                   1, 0, 1, 0, 1,
                                   1, 0, 1, 0, 1,
                                   1, 1, 1, 1, 1};

    Image image = {
        5,
        5,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {4, 0},
                {2, 1},
                {4, 1},
                {0, 2},
                {2, 2},
                {4, 2},
                {0, 3},
                {2, 3},
                {4, 3},
                {0, 4},
                {1, 4},
                {2, 4},
                {3, 4},
                {4, 4}
            },
            {0, 0},
            {4, 4}
        },
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(expected.size(), actual.size());

    ASSERT_EQ(expected[0].points.size(), actual[0].points.size());

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, Test2AdjacentPixelsGeneral) {
    // Setup Dependencies
    unsigned char imageData[9] = {0, 0, 1,
                                  1, 1, 0,
                                  0, 0, 0};

    Image image = {
        3,
        3,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {2, 0},
                {0, 1},
                {1, 1}
            },
            {0, 0},
            {2, 1}
        },
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, TestConvergingBlob) {
    // Setup Dependencies
    unsigned char imageData[20] = {1, 0, 0, 0, 1,
                                   1, 0, 1, 0, 1,
                                   0, 1, 1, 1, 0,
                                   0, 1, 1, 1, 0,};

    Image image = {
        5,
        4,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {0, 0},
                {4, 0},
                {0, 1},
                {2, 1},
                {4, 1},
                {1, 2},
                {2, 2},
                {3, 2},
                {1, 3},
                {2, 3},
                {3, 3},
            },
            {0, 0},
            {4, 3}
        },
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, Test2BlobsSimple) {
    // Setup Dependencies
    unsigned char imageData[9] = {0, 0, 1,
                                  0, 0, 0,
                                  1, 1, 0};

    Image image = {
        3,
        3,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {2, 0},
            },
            {2, 0},
            {2, 0}
        },
        {
            {
                {0, 2},
                {1, 2},
            },
            {0, 2},
            {1, 2}
        }
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, Test2BlobsGeneral) {
    // Setup Dependencies
    unsigned char imageData[25] = {0, 0, 0, 0, 1,
                                   0, 0, 0, 1, 1,
                                   1, 0, 0, 0, 1,
                                   1, 0, 1, 0, 0,
                                   1, 1, 1, 1, 1};

    Image image = {
        5,
        5,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {4, 0},
                {3, 1},
                {4, 1},
                {4, 2},
            },
            {3, 0},
            {4, 2}
        },
        {
            {
                {0, 2},
                {0, 3},
                {2, 3},
                {0, 4},
                {1, 4},
                {2, 4},
                {3, 4},
                {4, 4}
            },
            {0, 2},
            {4, 4}
        }
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(expected.size(), actual.size());

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, Test3BlobsGeneral) {
    // Setup Dependencies
    unsigned char imageData[36] = {0, 0, 1, 0, 1, 1,
                                   1, 1, 0, 0, 0, 1,
                                   0, 0, 1, 1, 0, 0,
                                   0, 0, 0, 0, 0, 1,
                                   0, 1, 0, 1, 0, 1,
                                   0, 1, 1, 1, 1, 1,};

    Image image = {
        6,
        6,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {2, 0},
                {0, 1},
                {1, 1},
                {2, 2},
                {3, 2}
            },
            {0, 0},
            {3, 2}
        },
        {
            {
                {4, 0},
                {5, 0},
                {5, 1}
            },
            {4, 0},
            {5, 1}
        },
        {
            {
                {5, 3},
                {1, 4},
                {3, 4},
                {5, 4},
                {1, 5},
                {2, 5},
                {3, 5},
                {4, 5},
                {5, 5}
            },
            {1, 3},
            {5, 5}
        },
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(expected.size(), actual.size());

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ConnectedComponentsTest, Test4BlobsGeneral) {
    // Setup Dependencies
    unsigned char imageData[25] = {0, 0, 1, 0, 1,
                                   1, 1, 0, 0, 1,
                                   1, 0, 0, 0, 0,
                                   0, 0, 1, 0, 1,
                                   1, 0, 0, 1, 1};

    Image image = {
        5,
        5,
        1,
        imageData,
    };

    Edges expected = {
        {
            {
                {2, 0},
                {0, 1},
                {1, 1},
                {0, 2},
            },
            {0, 0},
            {2, 2}
        },
        {
            {
                {4, 0},
                {4, 1},
            },
            {4, 0},
            {4, 1}
        },
        {
            {
                {0, 4}
            },
            {0, 4},
            {0, 4}
        },
        {
            {
                {2, 3},
                {4, 3},
                {3, 4},
                {4, 4}
            },
            {2, 3},
            {4, 4}
        }
    };

    Edges actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(expected.size(), actual.size());

    std::vector<testing::Matcher<Edge>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Edge& val) {
                    return EdgeEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

}  // namespace found
