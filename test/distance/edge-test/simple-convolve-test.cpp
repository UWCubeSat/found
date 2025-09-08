#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <algorithm>
#include <vector>

#include "test/common/common.hpp"

#include "src/common/style.hpp"

#include "src/distance/edge.hpp"
namespace found {

float identity_mask_data[9] = {
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

float blur_mask_data[9] = {
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

float negative_mask_data[9] = {
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

float float_mask_data[9] = {
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

float non_square_horizontal_mask_data[5] = {
    0, 0, 0, 1, 0
};
Mask non_square_horizontal_mask = {
    5,
    1,
    1,
    2,
    0,
    &non_square_horizontal_mask_data[0]
};

float non_square_vertical_mask_data[5] = {
    0, 0, 0, 1, 0
};
Mask non_square_vertical_mask = {
    1,
    5,
    1,
    0,
    2,
    &non_square_vertical_mask_data[0]
};

float multi_channel_mask_data[27] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0
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
    1,
    10,
    1,
    non_square_vertical_image_data
};

unsigned char multi_channel_image_data[27] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 1, 0, 0, 0, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};
Image multi_channel_image = {
    3,
    3,
    3,
    multi_channel_image_data
};

// Create dummy convolution edge detection algorithm for testing
class TestConvolutionEdgeDetectionAlgorithm : public ConvolutionalEdgeDetectionAlgorithm {
    public:
        // Inherit the constructor
        using ConvolutionalEdgeDetectionAlgorithm::ConvolutionalEdgeDetectionAlgorithm;
        //expose the ConvolveWithMask method
        using ConvolutionalEdgeDetectionAlgorithm::ConvolveWithMask;
        // Override the ApplyCriterion method to use a simple threshold
        bool ApplyCriterion(size_t index, Image &image) override {
            return image.image[index] > 0;
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

class IdentityMaskConvolveParameterizedTestFixture : public ::testing::TestWithParam<Image> {
};

TEST(SimpleConvolveTest, TestMultiChannelMaskSingleChannelImage) {
    EXPECT_THROW(multiChannelConvolve.ConvolveWithMask(three_by_three_image), std::invalid_argument);
}

TEST(SimpleConvolveTest, TestSingleChannelMaskMultiChannelImage) {
    EXPECT_THROW(identityConvolve.ConvolveWithMask(multi_channel_image), std::invalid_argument);
}

TEST_P(IdentityMaskConvolveParameterizedTestFixture, OutputMatchesInput) {
    Image image = GetParam();
    size_t num_pixels = image.width * image.height * image.channels;
    auto float_data = std::make_unique<float[]>(num_pixels);
    for (size_t i = 0; i < num_pixels; ++i) {
        float_data[i] = static_cast<float>(image.image[i]);
    }
    ConvolvedOutput expected = {
        image.width,
        image.height,
        image.channels,
        std::move(float_data)
    };
    ConvolvedOutput result = identityConvolve.ConvolveWithMask(image);
}

INSTANTIATE_TEST_SUITE_P(
    SimpleConvolveTest,
    IdentityMaskConvolveParameterizedTestFixture,
    ::testing::Values(
        single_pixel_image,
        three_by_three_image,
        non_square_horizontal_image,
        non_square_vertical_image
    )
);

} // namespace found