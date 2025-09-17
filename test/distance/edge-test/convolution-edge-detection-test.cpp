#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <algorithm>
#include <vector>

#include "test/common/common.hpp"
#include "test/common/constants/edge-constants.hpp"

#include "src/common/style.hpp"

#include "src/distance/edge.hpp"
namespace found {

///////////////////////////////
////// Test Convolution ///////
///////////////////////////////

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
    0
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
        TestConvolutionEdgeDetectionAlgorithm(Mask&&  mask, 
                                              int boxBasedMaskSize = 5, 
                                              decimal threshold = 1.f, 
                                              decimal channelCriterionRatio = 1.f, 
                                              decimal eigenValueRatio = .3f, 
                                              decimal edgeGradientRatio = .6f, 
                                              decimal spacePlanetGraytoneRatio = .3f) :
            ConvolutionEdgeDetectionAlgorithm(boxBasedMaskSize, std::move(mask), channelCriterionRatio, 
                                              eigenValueRatio, edgeGradientRatio, spacePlanetGraytoneRatio, 
                                              threshold) {}
        //expose the ConvolveWithMask method
        using ConvolutionEdgeDetectionAlgorithm::ConvolveWithMask;
        // expose the ApplyCriterion method
        using ConvolutionEdgeDetectionAlgorithm::ApplyCriterion;
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
    const Image* image;
    decimal* expected_data;
    size_t expected_size;
    int width;
    int height;
    int channels;
};

// Helper function to create expected_data_ptr from a decimal array
std::unique_ptr<decimal[]> makeExpectedPtr(const decimal* expected_data, size_t size) {
    auto expected_data_ptr = std::make_unique<decimal[]>(size);
    for (size_t i = 0; i < size; ++i) {
        expected_data_ptr[i] = expected_data[i];
    }
    return expected_data_ptr;
}

////// Test Channel Error Handling //////

TEST(ConvolutionEdgeDetectionTest, TestMultiChannelMaskSingleChannelImage) {
    ASSERT_THROW(multiChannelConvolve.ConvolveWithMask(three_by_three_image), std::invalid_argument);
}

TEST(ConvolutionEdgeDetectionTest, TestSingleChannelMaskMultiChannelImage) {
    ASSERT_THROW(identityConvolve.ConvolveWithMask(multi_channel_image), std::invalid_argument);
}

////// Test Multi-Channel Convolution //////

TEST(ConvolutionEdgeDetectionTest, TestMultiChannelConvolve) {
    /** 
     * Remember that the mask is flipped before convolution
     * This means that the first channel should be shifted left
     * The second channel should remain the same
     * The third channel should be shifted right
     */
   decimal expectedData[27] = {
        2, 1, 0,    3, 2, 1,    0, 3, 2,
        5, 4, 0,    6, 5, 4,    0, 6, 5,
        8, 7, 0,    9, 8, 7,    0, 9, 8
    };
    Tensor expected = {
        3,
        3,
        3,
        std::move(makeExpectedPtr(&expectedData[0], 27))
    };
    Tensor actual = multiChannelConvolve.ConvolveWithMask(multi_channel_image);

    ASSERT_TENSOR_EQ(expected, actual);
}

////// Parameterized Tests for Various Masks and Images //////

// Expected outputs for identity mask test 0 - 3
decimal expected_identity_single_pixel[1] = {0};
decimal expected_identity_three_by_three[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
decimal expected_identity_non_square_horizontal[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
decimal expected_identity_non_square_vertical[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
// Expected outputs for blur mask test 4 - 7
decimal expected_blur_single_pixel[1] = {0};
decimal expected_blur_three_by_three[9] = {12, 21, 16, 27, 45, 33, 24, 39, 28};
decimal expected_blur_non_square_horizontal[10] = {16, 27, 33, 39, 28, 16, 27, 33, 39, 28};
decimal expected_blur_non_square_vertical[10] = {10, 10, 21, 21, 33, 33, 45, 45, 34, 34};

// Expected outputs for negative mask test 8 - 11
decimal expected_negative_single_pixel[1] = {0};
decimal expected_negative_three_by_three[9] = {-12, -21, -16, -27, -45, -33, -24, -39, -28};
decimal expected_negative_non_square_horizontal[10] = {-16, -27, -33, -39, -28, -16, -27, -33, -39, -28};
decimal expected_negative_non_square_vertical[10] = {-10, -10, -21, -21, -33, -33, -45, -45, -34, -34};

// Expected outputs fordecimalmask test 12 - 15
decimal expected_float_single_pixel[1] = {0};
decimal expected_float_three_by_three[9] = {.5f, 1, 1.5f, 2, 2.5f, 3, 3.5f, 4, 4.5f};
decimal expected_float_non_square_horizontal[10] = {.5, 1, 1.5f, 2, 2.5f, 3, 3.5f, 4, 4.5f, 5};
decimal expected_float_non_square_vertical[10] = {.5, 1, 1.5f, 2, 2.5f, 3, 3.5f, 4, 4.5f, 5};

// Expected outputs for off-center mask test 16 - 19
decimal expected_off_center_single_pixel[1] = {0};
decimal expected_off_center_three_by_three[9] = {0, 0, 0, 0, 1, 2, 0, 4, 5};
decimal expected_off_center_non_square_horizontal[10] = {0, 0, 0, 0, 0, 0, 1, 2, 3, 4};
decimal expected_off_center_non_square_vertical[10] = {0, 0, 0, 1, 0, 3, 0, 5, 0, 7};

// Expected outputs for horizontal mask test 20 - 23
decimal expected_horizontal_single_pixel[1] = {0};
decimal expected_horizontal_three_by_three[9] = {10, 16, 22, 28, 43, 58, 46, 70, 94};
decimal expected_horizontal_non_square_horizontal[10] = {10, 20, 35, 44, 46, 40, 70, 110, 114, 106};
decimal expected_horizontal_non_square_vertical[10] = {7, 10, 17, 24, 27, 38, 37, 52, 47, 66};

// Expected outputs for vertical mask test 24 - 27
decimal expected_vertical_single_pixel[1] = {0};
decimal expected_vertical_three_by_three[9] = {18, 24, 30, 30, 39, 48, 42, 54, 66};
decimal expected_vertical_non_square_horizontal[10] = {15, 20, 25, 30, 35, 22, 29, 36, 43, 50};
decimal expected_vertical_non_square_vertical[10] = {14, 20, 30, 40, 55, 70, 74, 88, 80, 92};

// Parameterized test fixture
class ConvolveParameterizedTestFixture : public ::testing::TestWithParam<ConvolveTestParams> {};

TEST_P(ConvolveParameterizedTestFixture, OutputMatchesExpected) {
    const auto& param = GetParam();

    // Generate expected data
    Tensor expected = {
        param.width,
        param.height,
        param.channels,
        std::move(makeExpectedPtr(param.expected_data, param.expected_size))
    };

    Tensor actual = param.algorithm->ConvolveWithMask(*(param.image));

    // Print out algorithm (mask) and image names on failure
    ASSERT_TENSOR_EQ(expected, actual);
}

// Instantiate test suite
INSTANTIATE_TEST_SUITE_P(
    ConvolutionEdgeDetectionTest,
    ConvolveParameterizedTestFixture,
    ::testing::Values(
        // Identity mask tests tests 0 - 3
        ConvolveTestParams{&identityConvolve, &single_pixel_image, expected_identity_single_pixel, 1, 1, 1, 1},
        ConvolveTestParams{&identityConvolve, &three_by_three_image, expected_identity_three_by_three, 9, 3, 3, 1},
        ConvolveTestParams{&identityConvolve, &non_square_horizontal_image, expected_identity_non_square_horizontal, 10, 5, 2, 1},
        ConvolveTestParams{&identityConvolve, &non_square_vertical_image, expected_identity_non_square_vertical, 10, 2, 5, 1},

        // Blur mask tests tests 4 - 7
        ConvolveTestParams{&blurConvolve, &single_pixel_image, expected_blur_single_pixel, 1, 1, 1, 1},
        ConvolveTestParams{&blurConvolve, &three_by_three_image, expected_blur_three_by_three, 9, 3, 3, 1},
        ConvolveTestParams{&blurConvolve, &non_square_horizontal_image, expected_blur_non_square_horizontal, 10, 5, 2, 1},
        ConvolveTestParams{&blurConvolve, &non_square_vertical_image, expected_blur_non_square_vertical, 10, 2, 5, 1},

        // Negative mask tests tests 8 - 11
        ConvolveTestParams{&negativeConvolve, &single_pixel_image, expected_negative_single_pixel, 1, 1, 1, 1},
        ConvolveTestParams{&negativeConvolve, &three_by_three_image, expected_negative_three_by_three, 9, 3, 3, 1},
        ConvolveTestParams{&negativeConvolve, &non_square_horizontal_image, expected_negative_non_square_horizontal, 10, 5, 2, 1},
        ConvolveTestParams{&negativeConvolve, &non_square_vertical_image, expected_negative_non_square_vertical, 10, 2, 5, 1},

        // Float mask tests tests 12 - 15
        ConvolveTestParams{&floatConvolve, &single_pixel_image, expected_float_single_pixel, 1, 1, 1, 1},
        ConvolveTestParams{&floatConvolve, &three_by_three_image, expected_float_three_by_three, 9, 3, 3, 1},
        ConvolveTestParams{&floatConvolve, &non_square_horizontal_image, expected_float_non_square_horizontal, 10, 5, 2, 1},
        ConvolveTestParams{&floatConvolve, &non_square_vertical_image, expected_float_non_square_vertical, 10, 2, 5, 1},

        // Off-center mask tests tests 16 - 19
        ConvolveTestParams{&offCenterConvolve, &single_pixel_image, expected_off_center_single_pixel, 1, 1, 1, 1},
        ConvolveTestParams{&offCenterConvolve, &three_by_three_image, expected_off_center_three_by_three, 9, 3, 3, 1},
        ConvolveTestParams{&offCenterConvolve, &non_square_horizontal_image, expected_off_center_non_square_horizontal, 10, 5, 2, 1},
        ConvolveTestParams{&offCenterConvolve, &non_square_vertical_image, expected_off_center_non_square_vertical, 10, 2, 5, 1},

        // Horizontal mask tests tests 20 - 23
        ConvolveTestParams{&horizontalConvolve, &single_pixel_image, expected_horizontal_single_pixel, 1, 1, 1, 1},
        ConvolveTestParams{&horizontalConvolve, &three_by_three_image, expected_horizontal_three_by_three, 9, 3, 3, 1},
        ConvolveTestParams{&horizontalConvolve, &non_square_horizontal_image, expected_horizontal_non_square_horizontal, 10, 5, 2, 1},
        ConvolveTestParams{&horizontalConvolve, &non_square_vertical_image, expected_horizontal_non_square_vertical, 10, 2, 5, 1},

        // Vertical mask tests tests 24 - 27
        ConvolveTestParams{&verticalConvolve, &single_pixel_image, expected_vertical_single_pixel, 1, 1, 1, 1},
        ConvolveTestParams{&verticalConvolve, &three_by_three_image, expected_vertical_three_by_three, 9, 3, 3, 1},
        ConvolveTestParams{&verticalConvolve, &non_square_horizontal_image, expected_vertical_non_square_horizontal, 10, 5, 2, 1},
        ConvolveTestParams{&verticalConvolve, &non_square_vertical_image, expected_vertical_non_square_vertical, 10, 2, 5, 1}
    )
);

/////////////////////////////
////// Test Criterion ///////
/////////////////////////////

// Setup different gradients for criterion tests
decimal noEdgeTensorData[25] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0
};
Tensor noEdgeTensor = {
    5,
    5,
    1,
    std::move(makeExpectedPtr(&noEdgeTensorData[0], 25))
};
decimal verticalEdgeTensorData[25] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    10, 10, 10, 10, 10,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0
};
decimal horizontalEdgeTensorData[25] = {
    0, 0, 10, 0, 0,
    0, 0, 10, 0, 0,
    0, 0, 10, 0, 0,
    0, 0, 10, 0, 0,
    0, 0, 10, 0, 0
};
decimal pointEdgeTensorData[25] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 10, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0
};
decimal diagonalEdgeTensorData[25] = {
    10, 0, 0, 0, 0,
    0, 10, 0, 0, 0,
    0, 0, 10, 0, 0,
    0, 0, 0, 10, 0,
    0, 0, 0, 0, 10
};
decimal toManyEdgeTensorData[25] = {
    10, 0, 0, 0, 10,
    0, 10, 0, 10, 0,
    0, 0, 10, 0, 0,
    0, 10, 0, 10, 0,
    10, 0, 0, 0, 10
};
decimal onVerticalEdgeTensorData[25] = {
    10, 0, 0, 0, 0,
    10, 0, 0, 0, 0,
    10, 0, 0, 0, 0,
    10, 0, 0, 0, 0,
    10, 0, 0, 0, 0
};
decimal onHorizontalEdgeTensorData[25] = {
    10, 10, 10, 10, 10,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0
};

// setup classes for criterion tests
TestConvolutionEdgeDetectionAlgorithm fiveBoxCriterion(std::move(identity_mask), 5, -1.f, 1.f);
TestConvolutionEdgeDetectionAlgorithm multiChannelCriterion(std::move(identity_mask), 5, -1.f, .5f);

// Helper struct for parameterized tests
struct CriterionTestParams {
    TestConvolutionEdgeDetectionAlgorithm* algorithm;
    const Image* image;
    const decimal* tensor;
    const int width;
    const int height;
    const int channels;
    const int index;
    bool expected;
};

TEST(CriterionTest, TestAllChannelsMeetCriterion){
    decimal multiChannelTensorData[3] = {1,1,1};
    Tensor multiChannelTensor = {
        1,
        1,
        3,
        std::move(makeExpectedPtr(multiChannelTensorData, 3))
    };
    // Image we pass in does not matter since box criterion will skip 
    bool actual = fiveBoxCriterion.ApplyCriterion(0, multiChannelTensor, imageNoEdgeAllSpace);

    ASSERT_TRUE(actual);
}

TEST(CriterionTest, TestHalfChannelsMeetCriterion){
    decimal multiChannelTensorData[3] = {1,1,0};
    Tensor multiChannelTensor = {
        1,
        1,
        3,
        std::move(makeExpectedPtr(multiChannelTensorData, 3))
    };
    // Image we pass in does not matter since box criterion will skip 
    bool actual = multiChannelCriterion.ApplyCriterion(0, multiChannelTensor, imageNoEdgeAllSpace);

    bool expected = true;

    ASSERT_TRUE(actual);
}

TEST(CriterionTest, TestNoChannelsMeetCriterion){
    decimal multiChannelTensorData[3] = {0,0,0};
    Tensor multiChannelTensor = {
        1,
        1,
        3,
        std::move(makeExpectedPtr(multiChannelTensorData, 3))
    };
    // Image we pass in does not matter since box criterion will skip 
    bool actual = multiChannelCriterion.ApplyCriterion(0, multiChannelTensor, imageNoEdgeAllSpace);

    ASSERT_FALSE(actual);
}

// Parameterized test fixture for edge cases
class CriterionEdgeCaseParameterizedTestFixture : public ::testing::TestWithParam<CriterionTestParams> {};

TEST_P(CriterionEdgeCaseParameterizedTestFixture, RejectEdge) {
    const auto& param = GetParam();

    // Generate tensor from param data
    Tensor paramTensor = {
        param.width,
        param.height,
        param.channels,
        std::move(makeExpectedPtr(param.tensor, param.width * param.height * param.channels))
    };

    bool actual = param.algorithm->ApplyCriterion(param.index, paramTensor, *(param.image));

    // Print out algorithm (mask) and image names on failure
    ASSERT_EQ(param.expected, actual);
}

// Instantiate test suite
INSTANTIATE_TEST_SUITE_P(
    CriterionEdgeCaseTest,
    CriterionEdgeCaseParameterizedTestFixture,
    ::testing::Values(
        // No Edge Tensor tests - should reject as there's no edge
        CriterionTestParams{&fiveBoxCriterion, &imageNoEdgeAllSpace, &noEdgeTensorData[0], 5, 5, 1, 12, false},
        
        // Vertical Edge Tensor tests - should detect the horizontal line in the middle
        CriterionTestParams{&fiveBoxCriterion, &imageNoEdgeAllSpace, &verticalEdgeTensorData[0], 5, 5, 1, 12, true},
        
        // Horizontal Edge Tensor tests - should detect the vertical line in the middle
        CriterionTestParams{&fiveBoxCriterion, &imageNoEdgeAllSpace, &horizontalEdgeTensorData[0], 5, 5, 1, 12, true},
        
        // Point Edge Tensor tests - should detect the single point in the middle
        CriterionTestParams{&fiveBoxCriterion, &imageNoEdgeAllSpace, &pointEdgeTensorData[0], 5, 5, 1, 12, true},
        
        // Diagonal Edge Tensor tests - should detect the diagonal line
        CriterionTestParams{&fiveBoxCriterion, &imageNoEdgeAllSpace, &diagonalEdgeTensorData[0], 5, 5, 1, 12, true},
        
        // Too Many Edge Tensor tests - should detect multiple edges
        CriterionTestParams{&fiveBoxCriterion, &imageNoEdgeAllSpace, &toManyEdgeTensorData[0], 5, 5, 1, 12, true}
        
        // The last two tensors (onVerticalEdgeTensorData and onHorizontalEdgeTensorData) are excluded as requested
    )
);

} // namespace found
