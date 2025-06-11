#include <gtest/gtest.h>

#include <cmath>
#include <vector>
#include <algorithm>
#include <functional>

#include "test/common/common.hpp"

#include "src/common/style.hpp"

#include "src/distance/edge.hpp"

namespace found {

std::function<bool(uint64_t, const Image &)> criteria = [](uint64_t index, const Image &image) {
    return image.image[index] > 0;
};

MATCHER_P(ComponentEqual, expected, "") {
    return std::is_permutation(expected.points.begin(),
                               expected.points.end(),
                               arg.points.begin(),
                               arg.points.end()) &&
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

    Components expected = {
        {
            {0},
            {0, 0},
            {0, 0}
        }
    };


    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {1},
            {1, 0},
            {1, 0}
        }
    };


    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {0, 1},
            {0, 0},
            {1, 0}
        }
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {0, 3},
            {0, 0},
            {1, 1}
        },
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {1, 2},
            {0, 0},
            {1, 1}
        },
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {0, 2, 4, 6, 8},
            {0, 0},
            {2, 2}
        },
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {0, 2, 3, 4, 6},
            {0, 0},
            {2, 2}
        },
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {0, 1, 2, 4, 8},
            {0, 0},
            {2, 2}
        },
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {0, 4, 6, 7, 8},
            {0, 0},
            {2, 2}
        },
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {9, 10, 11, 12, 13, 17, 18, 19, 23, 24},
            {0, 1},
            {4, 4}
        },
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(expected.size(), actual.size());

    ASSERT_EQ(expected[0].points.size(), actual[0].points.size());

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {4, 5, 7, 9, 10, 12, 13, 15, 16, 17, 18},
            {0, 0},
            {4, 3}
        },
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(expected.size(), actual.size());

    ASSERT_EQ(expected[0].points.size(), actual[0].points.size());

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {9, 10, 11, 12, 13, 15},
            {0, 1},
            {4, 3}
        },
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(expected.size(), actual.size());

    ASSERT_EQ(expected[0].points.size(), actual[0].points.size());

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {0, 2, 4, 5, 7, 9, 10, 12, 14, 15, 17, 19, 20, 21, 22, 23, 24},
            {0, 0},
            {4, 4}
        },
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(expected.size(), actual.size());

    ASSERT_EQ(expected[0].points.size(), actual[0].points.size());

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {4, 7, 9, 10, 12, 14, 15, 17, 19, 20, 21, 22, 23, 24},
            {0, 0},
            {4, 4}
        },
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(expected.size(), actual.size());

    ASSERT_EQ(expected[0].points.size(), actual[0].points.size());

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {2, 3, 4},
            {0, 0},
            {2, 1}
        },
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {0, 4, 5, 7, 9, 11, 12, 13, 16, 17, 18},
            {0, 0},
            {4, 3}
        },
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {2},
            {2, 0},
            {2, 0}
        },
        {
            {6, 7},
            {0, 2},
            {1, 2}
        }
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {4, 8, 9, 14},
            {3, 0},
            {4, 2}
        },
        {
            {10, 15, 17, 20, 21, 22, 23, 24},
            {0, 2},
            {4, 4}
        }
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(expected.size(), actual.size());

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {2, 6, 7, 14, 15},
            {0, 0},
            {3, 2}
        },
        {
            {4, 5, 11},
            {4, 0},
            {5, 1}
        },
        {
            {23, 25, 27, 29, 31, 32, 33, 34, 35},
            {1, 3},
            {5, 5}
        },
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(expected.size(), actual.size());

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
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

    Components expected = {
        {
            {2, 5, 6, 10},
            {0, 0},
            {2, 2}
        },
        {
            {4, 9},
            {4, 0},
            {4, 1}
        },
        {
            {20},
            {0, 4},
            {0, 4}
        },
        {
            {17, 19, 23, 24},
            {2, 3},
            {4, 4}
        }
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(expected.size(), actual.size());

    std::vector<testing::Matcher<Component>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
                [](const Component& val) {
                    return ComponentEqual(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

}  // namespace found
