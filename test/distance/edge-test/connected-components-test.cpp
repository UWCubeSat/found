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

// Helper function to convert (x, y) coordinates to index
inline uint64_t xyToIndex(int width, int x, int y) {
    return y * width + x;
}

MATCHER_P(ComponentEqual, expected, "") {
    return expected.points == arg.points &&
           arg.upperLeftIndex == expected.upperLeftIndex &&
           arg.lowerRightIndex == expected.lowerRightIndex;
}

TEST(ConnectedComponentsTest, TestInvalidImage) {
    // We can't actually have an empty array, but
    // we need to pretend like it is one
    unsigned char imageData[1] = {0};

    Image image = {
        1,
        -1,  // Negative height
        0,
        imageData,
    };

    ASSERT_ANY_THROW(ConnectedComponentsAlgorithm(image, criteria));
}

TEST(ConnectedComponentsTest, TestInvalidImageNegativeWidth) {
    // Test negative width case
    unsigned char imageData[1] = {0};

    Image image = {
        -1,  // Negative width
        1,
        0,
        imageData,
    };

    ASSERT_ANY_THROW(ConnectedComponentsAlgorithm(image, criteria));
}

TEST(ConnectedComponentsTest, TestEmptyImage) {
    // We can't actually have an empty array, but
    // we need to pretend like it is one
    unsigned char imageData[1] = {0};

    Image image = {
        0,
        0,
        0,
        imageData,
    };

    Components expected = {
        {
        }
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(static_cast<size_t>(0), actual.size());
}

TEST(ConnectedComponentsTest, TestEmptyImageWidth) {
    // Test edge case where width is 0 but height is not
    unsigned char imageData[1] = {0};

    Image image = {
        0,
        10,
        1,
        imageData,
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);
    ASSERT_EQ(static_cast<size_t>(0), actual.size());
}

TEST(ConnectedComponentsTest, TestEmptyImageHeight) {
    // Test edge case where height is 0 but width is not
    unsigned char imageData[1] = {0};

    Image image = {
        10,
        0,
        1,
        imageData,
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);
    ASSERT_EQ(static_cast<size_t>(0), actual.size());
}

TEST(ConnectedComponentsTest, TestImageTooLarge) {
    // Test the maximum dimension check
    unsigned char imageData[1] = {0};

    Image image = {
        100001,  // Exceeds maximum width
        100,
        1,
        imageData,
    };

    ASSERT_ANY_THROW(ConnectedComponentsAlgorithm(image, criteria));

    Image image2 = {
        100,
        100001,  // Exceeds maximum height
        1,
        imageData,
    };

    ASSERT_ANY_THROW(ConnectedComponentsAlgorithm(image2, criteria));
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
            0,  // upperLeftIndex: y=0, x=0 -> index = 0*2 + 0 = 0
            0   // lowerRightIndex: y=0, x=0 -> index = 0*2 + 0 = 0
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
            1,  // upperLeftIndex: y=0, x=1 -> index = 0*2 + 1 = 1
            1   // lowerRightIndex: y=0, x=1 -> index = 0*2 + 1 = 1
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
            0,  // upperLeftIndex: y=0, x=0 -> index = 0*2 + 0 = 0
            1   // lowerRightIndex: y=0, x=1 -> index = 0*2 + 1 = 1
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
            xyToIndex(2, 0, 0),  // (0, 0)
            xyToIndex(2, 1, 1)  // (1, 1)
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
            xyToIndex(2, 0, 0),  // (0, 0)
            xyToIndex(2, 1, 1)  // (1, 1)
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
            xyToIndex(3, 0, 0),  // (0, 0)
            xyToIndex(3, 2, 2)  // (2, 2)
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
            xyToIndex(3, 0, 0),  // (0, 0)
            xyToIndex(3, 2, 2)  // (2, 2)
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
            xyToIndex(3, 0, 0),  // (0, 0)
            xyToIndex(3, 2, 2)  // (2, 2)
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
            xyToIndex(3, 0, 0),  // (0, 0)
            xyToIndex(3, 2, 2)  // (2, 2)
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
            xyToIndex(5, 0, 1),  // (0, 1)
            xyToIndex(5, 4, 4)  // (4, 4)
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
            xyToIndex(5, 0, 0),  // (0, 0)
            xyToIndex(5, 4, 3)  // (4, 3)
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
            xyToIndex(5, 0, 1),  // (0, 1)
            xyToIndex(5, 4, 3)  // (4, 3)
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
            xyToIndex(5, 0, 0),  // (0, 0)
            xyToIndex(5, 4, 4)  // (4, 4)
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
            xyToIndex(5, 0, 0),  // (0, 0)
            xyToIndex(5, 4, 4)  // (4, 4)
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
            xyToIndex(3, 0, 0),  // (0, 0)
            xyToIndex(3, 2, 1)  // (2, 1)
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
            xyToIndex(5, 0, 0),  // (0, 0)
            xyToIndex(5, 4, 3)  // (4, 3)
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
            xyToIndex(3, 2, 0),  // (2, 0)
            xyToIndex(3, 2, 0)  // (2, 0)
        },
        {
            {6, 7},
            xyToIndex(3, 0, 2),  // (0, 2)
            xyToIndex(3, 1, 2)  // (1, 2)
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
            xyToIndex(5, 3, 0),  // (3, 0)
            xyToIndex(5, 4, 2)  // (4, 2)
        },
        {
            {10, 15, 17, 20, 21, 22, 23, 24},
            xyToIndex(5, 0, 2),  // (0, 2)
            xyToIndex(5, 4, 4)  // (4, 4)
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
            xyToIndex(6, 0, 0),  // (0, 0)
            xyToIndex(6, 3, 2)  // (3, 2)
        },
        {
            {4, 5, 11},
            xyToIndex(6, 4, 0),  // (4, 0)
            xyToIndex(6, 5, 1)  // (5, 1)
        },
        {
            {23, 25, 27, 29, 31, 32, 33, 34, 35},
            xyToIndex(6, 1, 3),  // (1, 3)
            xyToIndex(6, 5, 5)  // (5, 5)
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
            xyToIndex(5, 0, 0),  // (0, 0)
            xyToIndex(5, 2, 2)  // (2, 2)
        },
        {
            {4, 9},
            xyToIndex(5, 4, 0),  // (4, 0)
            xyToIndex(5, 4, 1)  // (4, 1)
        },
        {
            {20},
            xyToIndex(5, 0, 4),  // (0, 4)
            xyToIndex(5, 0, 4)  // (0, 4)
        },
        {
            {17, 19, 23, 24},
            xyToIndex(5, 2, 3),  // (2, 3)
            xyToIndex(5, 4, 4)  // (4, 4)
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

TEST(ConnectedComponentsTest, TestBoundsNoUpdateWhenPixelInside) {
    // Test case where adding a pixel doesn't change bounds (pixel is inside bounding box)
    // This covers the needUpdate = false branches in UpdateComponent
    unsigned char imageData[9] = {1, 1, 1,
                                  1, 0, 1,
                                  1, 1, 1};

    Image image = {
        3,
        3,
        1,
        imageData,
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);
    
    // Should have 1 component with all 8 pixels
    ASSERT_EQ(static_cast<size_t>(1), actual.size());
    ASSERT_EQ(static_cast<size_t>(8), actual[0].points.size());
    // Bounds should be from (0,0) to (2,2) - covering the entire 3x3 area
    ASSERT_EQ(xyToIndex(3, 0, 0), actual[0].upperLeftIndex);
    ASSERT_EQ(xyToIndex(3, 2, 2), actual[0].lowerRightIndex);
}

TEST(ConnectedComponentsTest, TestMergeBoundsNoChange) {
    // Test case where merging components doesn't change bounds
    // This covers the branches where newMinX == lowestULX && newMinY == lowestULY
    // and newMaxX == lowestLRX && newMaxY == lowestLRY
    // Create a pattern where components merge but one's bounds are contained in the other
    unsigned char imageData[25] = {1, 0, 0, 0, 1,
                                   0, 1, 1, 1, 0,
                                   0, 1, 0, 1, 0,
                                   0, 1, 1, 1, 0,
                                   1, 0, 0, 0, 1};

    Image image = {
        5,
        5,
        1,
        imageData,
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);
    
    // Should have 1 component (all pixels connect)
    ASSERT_EQ(static_cast<size_t>(1), actual.size());
    // Bounds should cover entire area
    ASSERT_EQ(xyToIndex(5, 0, 0), actual[0].upperLeftIndex);
    ASSERT_EQ(xyToIndex(5, 4, 4), actual[0].lowerRightIndex);
}

TEST(ConnectedComponentsTest, TestBoundsUpdateOnlyX) {
    // Test case where only X coordinate changes (not Y)
    // Pixels are connected horizontally
    unsigned char imageData[6] = {1, 1, 1,
                                  0, 0, 0};

    Image image = {
        3,
        2,
        1,
        imageData,
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);
    
    ASSERT_EQ(static_cast<size_t>(1), actual.size());
    ASSERT_EQ(static_cast<size_t>(3), actual[0].points.size());
    // Bounds should be from (0,0) to (2,0)
    ASSERT_EQ(xyToIndex(3, 0, 0), actual[0].upperLeftIndex);
    ASSERT_EQ(xyToIndex(3, 2, 0), actual[0].lowerRightIndex);
}

TEST(ConnectedComponentsTest, TestBoundsUpdateOnlyY) {
    // Test case where only Y coordinate changes (not X)
    // Pixels are connected vertically
    unsigned char imageData[6] = {1, 0, 0,
                                  1, 0, 0};

    Image image = {
        3,
        2,
        1,
        imageData,
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    ASSERT_EQ(static_cast<size_t>(1), actual.size());
    ASSERT_EQ(static_cast<size_t>(2), actual[0].points.size());
    // Bounds should be from (0,0) to (0,1)
    ASSERT_EQ(xyToIndex(3, 0, 0), actual[0].upperLeftIndex);
    ASSERT_EQ(xyToIndex(3, 0, 1), actual[0].lowerRightIndex);
}

TEST(ConnectedComponentsTest, TestBoundsUpdateMinYWhenMerging) {
    // Test case to cover y < upperLeftY branch in UpdateComponent
    // Since we process top-to-bottom, this branch is only reachable if
    // a component's upperLeftY gets set incorrectly, or if we manually
    // construct a scenario. However, with sequential processing, this
    // branch may be unreachable in practice.
    // 
    // This test creates a pattern where pixels connect in a way that
    // might trigger the branch, though it may not be reachable.
    unsigned char imageData[12] = {1, 0, 0, 0,
                                    0, 1, 0, 0,
                                    0, 0, 1, 0};

    Image image = {
        4,
        3,
        1,
        imageData,
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    // Should have 1 component (diagonal connection)
    ASSERT_EQ(static_cast<size_t>(1), actual.size());
    ASSERT_EQ(xyToIndex(4, 0, 0), actual[0].upperLeftIndex);
    ASSERT_EQ(xyToIndex(4, 2, 2), actual[0].lowerRightIndex);
}

TEST(ConnectedComponentsTest, TestBoundsUpdateMinXWhenMerging) {
    // Test case to cover x < upperLeftX branch in UpdateComponent
    // Similar to TestBoundsUpdateMinYWhenMerging, this may be unreachable
    // with sequential processing, but we test it anyway.
    // Note: With 4-connectivity, diagonal pixels don't connect, so we need
    // a pattern where pixels connect horizontally/vertically.
    unsigned char imageData[12] = {0, 0, 1, 1,
                                    0, 1, 1, 0,
                                    1, 1, 0, 0};

    Image image = {
        4,
        3,
        1,
        imageData,
    };

    Components actual = ConnectedComponentsAlgorithm(image, criteria);

    // Should have 1 component (connected via horizontal/vertical)
    ASSERT_EQ(static_cast<size_t>(1), actual.size());
    // Verify it has all 6 pixels
    ASSERT_EQ(static_cast<size_t>(6), actual[0].points.size());
    // Bounds should cover all pixels - verify they're correct
    uint64_t upperLeftX = actual[0].upperLeftIndex % 4;
    uint64_t upperLeftY = actual[0].upperLeftIndex / 4;
    uint64_t lowerRightX = actual[0].lowerRightIndex % 4;
    uint64_t lowerRightY = actual[0].lowerRightIndex / 4;
    ASSERT_EQ(0u, upperLeftX);
    ASSERT_EQ(0u, upperLeftY);
    ASSERT_EQ(3u, lowerRightX);
    ASSERT_EQ(2u, lowerRightY);
}

}  // namespace found
