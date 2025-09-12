#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <algorithm>
#include <vector>

#include "test/common/common.hpp"

#include "src/common/style.hpp"

#include "src/distance/edge.hpp"
namespace found {

decimal identity_mask_data[9] = {
    0, 0, 0,
    0, 1, 0,
    0, 0, 0
};
Mask identity_mask = {
    3,
    3,
    1,
    1,
    1,
    &identity_mask_data[0]
};

decimal blur_mask_data[9] = {
    1, 1, 1,
    1, 1, 1,
    1, 1, 1
};
Mask blur_mask = {
    3,
    3,
    1,
    1,
    1,
    &blur_mask_data[0]
};

decimal negative_mask_data[9] = {
    -1, -1, -1,
    -1, -1, -1,
    -1, -1, -1
};
Mask negative_mask = {
    3,
    3,
    1,
    1,
    1,
    &negative_mask_data[0]
};

decimal float_mask_data[9] = {
    0, 0, 0,
    0, 0.5f, 0,
    0, 0, 0
};
Mask float_mask = {
    3,
    3,
    1,
    1,
    1,
    &float_mask_data[0]
};

Mask off_center_mask = {
    3,
    3,
    1,
    0,
    0,
    &identity_mask_data[0]
};

decimal non_square_horizontal_mask_data[5] = {
    1, 2, 3, 4, 5
};
Mask non_square_horizontal_mask = {
    5,
    1,
    1,
    2,
    0,
    &non_square_horizontal_mask_data[0]
};

decimal non_square_vertical_mask_data[5] = {
    1, 2, 3, 4, 5
};
Mask non_square_vertical_mask = {
    1,
    5,
    1,
    0,
    2,
    &non_square_vertical_mask_data[0]
};

decimal multi_channel_mask_data[27] = {
    0, 0, 0,    0, 0, 0,    0, 0, 0,
    1, 0, 0,    0, 1, 0,    0, 0, 1,
    0, 0, 0,    0, 0, 0,    0, 0, 0
};
Mask multi_channel_mask = {
    3,
    3,
    3,
    1,
    1,
    &multi_channel_mask_data[0]
};

// Setup different images for convolution tests
unsigned char single_pixel_image_data[1] = {
    1
};
Image single_pixel_image = {
    1,
    1,
    1,
    single_pixel_image_data
};

unsigned char three_by_three_image_data[9] = {
    1, 2, 3,
    4, 5, 6,
    7, 8, 9
};
Image three_by_three_image = {
    3,
    3,
    1,
    three_by_three_image_data
};

unsigned char non_square_horizontal_image_data[10] = {
    1, 2, 3, 4, 5,
    6, 7, 8, 9, 10
};
Image non_square_horizontal_image = {
    5,
    2,
    1,
    non_square_horizontal_image_data
};

unsigned char non_square_vertical_image_data[10] = {
    1, 2,
    3, 4,
    5, 6,
    7, 8,
    9, 10
};
Image non_square_vertical_image = {
    2,
    5,
    1,
    non_square_vertical_image_data
};

unsigned char multi_channel_image_data[27] = {
    1, 1, 1, 2, 2, 2, 3, 3, 3,
    4, 4, 4, 5, 5, 5, 6, 6, 6,
    7, 7, 7, 8, 8, 8, 9, 9, 9
};
Image multi_channel_image = {
    3,
    3,
    3,
    multi_channel_image_data
};

// Create dummy convolution edge detection algorithm for testing
class TestConvolutionEdgeDetectionAlgorithm : public ConvolutionEdgeDetectionAlgorithm {
    public:
        // Inherit the constructor
        TestConvolutionEdgeDetectionAlgorithm(Mask&&  mask, size_t boxBasedMaskSize = 5, decimal channelCriterionRatio = 1.f,
     decimal eigenValueRatio = .3f, decimal edgeGradientRatio = .6f, decimal spacePlanetGraytoneRatio = .3f) :
            ConvolutionEdgeDetectionAlgorithm(boxBasedMaskSize, std::move(mask), channelCriterionRatio, 
            eigenValueRatio, edgeGradientRatio, spacePlanetGraytoneRatio) {}
        //expose the ConvolveWithMask method
        using ConvolutionEdgeDetectionAlgorithm::ConvolveWithMask;
        // expose the ApplyCriterion method
        using ConvolutionEdgeDetectionAlgorithm::ApplyCriterion;
        // expose the BoxBasedOutlierCriterion method
        Points Run(const Image &image) override {
            (void)image;
            return Points{};
        }

};

// Initialize different convolution algorithms
TestConvolutionEdgeDetectionAlgorithm identityConvolve(std::move(identity_mask));
TestConvolutionEdgeDetectionAlgorithm blurConvolve(std::move(blur_mask));
TestConvolutionEdgeDetectionAlgorithm negativeConvolve(std::move(negative_mask));
TestConvolutionEdgeDetectionAlgorithm floatConvolve(std::move(float_mask));
TestConvolutionEdgeDetectionAlgorithm offCenterConvolve(std::move(off_center_mask));
TestConvolutionEdgeDetectionAlgorithm horizontalConvolve(std::move(non_square_horizontal_mask));
TestConvolutionEdgeDetectionAlgorithm verticalConvolve(std::move(non_square_vertical_mask));
TestConvolutionEdgeDetectionAlgorithm multiChannelConvolve(std::move(multi_channel_mask));

// Helper struct for parameterized tests
struct ConvolveTestParams {
    TestConvolutionEdgeDetectionAlgorithm* algorithm;
    const char* algorithm_name;
    const Image* image;
    const char* image_name;
    decimal* expected_data;
    size_t expected_size;
    int width;
    int height;
    int channels;
};

// Helper function to create expected_data_ptr from a decimal array
std::unique_ptr<decimal[]> make_expected_ptr(const decimal* expected_data, size_t size) {
    auto expected_data_ptr = std::make_unique<decimal[]>(size);
    for (size_t i = 0; i < size; ++i) {
        expected_data_ptr[i] = expected_data[i];
    }
    return expected_data_ptr;
}

class IdentityMaskConvolveParameterizedTestFixture : public ::testing::TestWithParam<Image> {
};

////// Test Channel Error Handling //////

TEST(ConvolutionEdgeDetectionTest, TestMultiChannelMaskSingleChannelImage) {
    EXPECT_THROW(multiChannelConvolve.ConvolveWithMask(three_by_three_image), std::invalid_argument);
}

TEST(ConvolutionEdgeDetectionTest, TestSingleChannelMaskMultiChannelImage) {
    EXPECT_THROW(identityConvolve.ConvolveWithMask(multi_channel_image), std::invalid_argument);
}

////// Test Multi-Channel Convolution //////

TEST(ConvolutionEdgeDetectionTest, TestMultiChannelConvolve) {
    /** 
     * Remember that the mask is flipped before convolution
     * This means that the first channel should be shifted left
     * The second channel should remain the same
     * The third channel should be shifted right
     */
   decimal expected_data[27] = {
        2, 1, 0,    3, 2, 1,    0, 3, 2,
        5, 4, 0,    6, 5, 4,    0, 6, 5,
        8, 7, 0,    9, 8, 7,    0, 9, 8
    };
    auto expected_data_ptr = std::make_unique<decimal[]>(27);
    for (size_t i = 0; i < 27; ++i) {
        expected_data_ptr[i] = expected_data[i];
    }
    Tensor expected = {
        3,
        3,
        3,
        std::move(expected_data_ptr)
    };
    Tensor actual = multiChannelConvolve.ConvolveWithMask(multi_channel_image);

    ASSERT_IMAGE_EQ(expected, actual);
}

////// Parameterized Tests for Various Masks and Images //////

// Expected outputs for identity mask test 0 - 3
decimal expected_identity_single_pixel[1] = {1};
decimal expected_identity_three_by_three[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
decimal expected_identity_non_square_horizontal[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
decimal expected_identity_non_square_vertical[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
// Expected outputs for blur mask test 4 - 7
decimal expected_blur_single_pixel[1] = {1};
decimal expected_blur_three_by_three[9] = {12, 21, 16, 27, 45, 33, 24, 39, 28};
decimal expected_blur_non_square_horizontal[10] = {16, 27, 33, 39, 28, 16, 27, 33, 39, 28};
decimal expected_blur_non_square_vertical[10] = {10, 10, 21, 21, 33, 33, 45, 45, 34, 34};

// Expected outputs for negative mask test 8 - 11
decimal expected_negative_single_pixel[1] = {-1};
decimal expected_negative_three_by_three[9] = {-12, -21, -16, -27, -45, -33, -24, -39, -28};
decimal expected_negative_non_square_horizontal[10] = {-16, -27, -33, -39, -28, -16, -27, -33, -39, -28};
decimal expected_negative_non_square_vertical[10] = {-10, -10, -21, -21, -33, -33, -45, -45, -34, -34};

// Expected outputs fordecimalmask test 12 - 15
decimal expected_float_single_pixel[1] = {0.5f};
decimal expected_float_three_by_three[9] = {.5f, 1, 1.5f, 2, 2.5f, 3, 3.5f, 4, 4.5f};
decimal expected_float_non_square_horizontal[10] = {.5, 1, 1.5f, 2, 2.5f, 3, 3.5f, 4, 4.5f, 5};
decimal expected_float_non_square_vertical[10] = {.5, 1, 1.5f, 2, 2.5f, 3, 3.5f, 4, 4.5f, 5};

// Expected outputs for off-center mask test 16 - 19
decimal expected_off_center_single_pixel[1] = {0};
decimal expected_off_center_three_by_three[9] = {0, 0, 0, 0, 1, 2, 0, 4, 5};
decimal expected_off_center_non_square_horizontal[10] = {0, 0, 0, 0, 0, 0, 1, 2, 3, 4};
decimal expected_off_center_non_square_vertical[10] = {0, 0, 0, 1, 0, 3, 0, 5, 0, 7};

// Expected outputs for horizontal mask test 20 - 23
decimal expected_horizontal_single_pixel[1] = {3};
decimal expected_horizontal_three_by_three[9] = {10, 16, 22, 28, 43, 58, 46, 70, 94};
decimal expected_horizontal_non_square_horizontal[10] = {10, 20, 35, 44, 46, 40, 70, 110, 114, 106};
decimal expected_horizontal_non_square_vertical[10] = {7, 10, 17, 24, 27, 38, 37, 52, 47, 66};

// Expected outputs for vertical mask test 24 - 27
decimal expected_vertical_single_pixel[1] = {3};
decimal expected_vertical_three_by_three[9] = {18, 24, 30, 30, 39, 48, 42, 54, 66};
decimal expected_vertical_non_square_horizontal[10] = {15, 20, 25, 30, 35, 22, 29, 36, 43, 50};
decimal expected_vertical_non_square_vertical[10] = {14, 20, 30, 40, 55, 70, 74, 88, 80, 92};

// Parameterized test fixture
class ConvolveParameterizedTestFixture : public ::testing::TestWithParam<ConvolveTestParams> {};

TEST_P(ConvolveParameterizedTestFixture, OutputMatchesExpected) {
    const auto& param = GetParam();

    // Generate expected data
    auto expected_data_ptr = make_expected_ptr(param.expected_data, param.expected_size);
    Tensor expected = {
        param.width,
        param.height,
        param.channels,
        std::move(expected_data_ptr)
    };

    Tensor actual = param.algorithm->ConvolveWithMask(*(param.image));

    // Print out algorithm (mask) and image names on failure
    ASSERT_IMAGE_EQ(expected, actual);
}

// Instantiate test suite
INSTANTIATE_TEST_SUITE_P(
    ConvolutionEdgeDetectionTest,
    ConvolveParameterizedTestFixture,
    ::testing::Values(
        // Identity mask tests tests 0 - 3
        ConvolveTestParams{&identityConvolve, "Identity", &single_pixel_image, "SinglePixel", expected_identity_single_pixel, 1, 1, 1, 1},
        ConvolveTestParams{&identityConvolve, "Identity", &three_by_three_image, "ThreeByThree", expected_identity_three_by_three, 9, 3, 3, 1},
        ConvolveTestParams{&identityConvolve, "Identity", &non_square_horizontal_image, "NonSquareHorizontal", expected_identity_non_square_horizontal, 10, 5, 2, 1},
        ConvolveTestParams{&identityConvolve, "Identity", &non_square_vertical_image, "NonSquareVertical", expected_identity_non_square_vertical, 10, 2, 5, 1},

        // Blur mask tests tests 4 - 7
        ConvolveTestParams{&blurConvolve, "Blur", &single_pixel_image, "SinglePixel", expected_blur_single_pixel, 1, 1, 1, 1},
        ConvolveTestParams{&blurConvolve, "Blur", &three_by_three_image, "ThreeByThree", expected_blur_three_by_three, 9, 3, 3, 1},
        ConvolveTestParams{&blurConvolve, "Blur", &non_square_horizontal_image, "NonSquareHorizontal", expected_blur_non_square_horizontal, 10, 5, 2, 1},
        ConvolveTestParams{&blurConvolve, "Blur", &non_square_vertical_image, "NonSquareVertical", expected_blur_non_square_vertical, 10, 2, 5, 1},

        // Negative mask tests tests 8 - 11
        ConvolveTestParams{&negativeConvolve, "Negative", &single_pixel_image, "SinglePixel", expected_negative_single_pixel, 1, 1, 1, 1},
        ConvolveTestParams{&negativeConvolve, "Negative", &three_by_three_image, "ThreeByThree", expected_negative_three_by_three, 9, 3, 3, 1},
        ConvolveTestParams{&negativeConvolve, "Negative", &non_square_horizontal_image, "NonSquareHorizontal", expected_negative_non_square_horizontal, 10, 5, 2, 1},
        ConvolveTestParams{&negativeConvolve, "Negative", &non_square_vertical_image, "NonSquareVertical", expected_negative_non_square_vertical, 10, 2, 5, 1},

        // Float mask tests tests 12 - 15
        ConvolveTestParams{&floatConvolve, "Float", &single_pixel_image, "SinglePixel", expected_float_single_pixel, 1, 1, 1, 1},
        ConvolveTestParams{&floatConvolve, "Float", &three_by_three_image, "ThreeByThree", expected_float_three_by_three, 9, 3, 3, 1},
        ConvolveTestParams{&floatConvolve, "Float", &non_square_horizontal_image, "NonSquareHorizontal", expected_float_non_square_horizontal, 10, 5, 2, 1},
        ConvolveTestParams{&floatConvolve, "Float", &non_square_vertical_image, "NonSquareVertical", expected_float_non_square_vertical, 10, 2, 5, 1},

        // Off-center mask tests tests 16 - 19
        ConvolveTestParams{&offCenterConvolve, "OffCenter", &single_pixel_image, "SinglePixel", expected_off_center_single_pixel, 1, 1, 1, 1},
        ConvolveTestParams{&offCenterConvolve, "OffCenter", &three_by_three_image, "ThreeByThree", expected_off_center_three_by_three, 9, 3, 3, 1},
        ConvolveTestParams{&offCenterConvolve, "OffCenter", &non_square_horizontal_image, "NonSquareHorizontal", expected_off_center_non_square_horizontal, 10, 5, 2, 1},
        ConvolveTestParams{&offCenterConvolve, "OffCenter", &non_square_vertical_image, "NonSquareVertical", expected_off_center_non_square_vertical, 10, 2, 5, 1},

        // Horizontal mask tests tests 20 - 23
        ConvolveTestParams{&horizontalConvolve, "Horizontal", &single_pixel_image, "SinglePixel", expected_horizontal_single_pixel, 1, 1, 1, 1},
        ConvolveTestParams{&horizontalConvolve, "Horizontal", &three_by_three_image, "ThreeByThree", expected_horizontal_three_by_three, 9, 3, 3, 1},
        ConvolveTestParams{&horizontalConvolve, "Horizontal", &non_square_horizontal_image, "NonSquareHorizontal", expected_horizontal_non_square_horizontal, 10, 5, 2, 1},
        ConvolveTestParams{&horizontalConvolve, "Horizontal", &non_square_vertical_image, "NonSquareVertical", expected_horizontal_non_square_vertical, 10, 2, 5, 1},

        // Vertical mask tests tests 24 - 27
        ConvolveTestParams{&verticalConvolve, "Vertical", &single_pixel_image, "SinglePixel", expected_vertical_single_pixel, 1, 1, 1, 1},
        ConvolveTestParams{&verticalConvolve, "Vertical", &three_by_three_image, "ThreeByThree", expected_vertical_three_by_three, 9, 3, 3, 1},
        ConvolveTestParams{&verticalConvolve, "Vertical", &non_square_horizontal_image, "NonSquareHorizontal", expected_vertical_non_square_horizontal, 10, 5, 2, 1},
        ConvolveTestParams{&verticalConvolve, "Vertical", &non_square_vertical_image, "NonSquareVertical", expected_vertical_non_square_vertical, 10, 2, 5, 1}
    )
);

} // namespace found
