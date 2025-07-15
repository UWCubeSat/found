// #include <gtest/gtest.h>

// #include "test/common/common.hpp"

// #include "src/common/style.hpp"

// #include "src/distance/edge.hpp"

// CREATE_IMG(int width, int height, int channels, const unsigned char* data) {
//     Image img;
//     img.width = width;
//     img.height = height;
//     img.channels = channels;
//     if (width > 0 && height > 0 && channels > 0) {
//         img.image = new unsigned char[width * height * channels];
//         if (data) {
//             for (int i = 0; i < width * height * channels; ++i) {
//                 img.image[i] = data[i];
//             }
//         } else {
//             for (int i = 0; i < width * height * channels; ++i) {
//                 img.image[i] = 0;
//             }
//         }
//     } else {
//         img.image = nullptr;
//     }
//     return img;
// }

// DESTROY_IMG(Image& img) {
//     delete[] img.image;
//     img.image = nullptr;
// }

// IMAGES_EQ(const Image& a, const Image& b) {
//     EXPECT_EQ(a.width, b.width);
//     EXPECT_EQ(a.height, b.height);
//     EXPECT_EQ(a.channels, b.channels);
//     if (a.width != b.width || a.height != b.height || a.channels != b.channels) {
//         return false;
//     }
//     if (a.image == nullptr && b.image == nullptr) {
//         return true;
//     }
//     if (a.image == nullptr || b.image == nullptr) {
//         return false;
//     }
//     for (int i = 0; i < a.width * a.height * a.channels; ++i) {
//         if (a.image[i] != b.image[i]) {
//             return false;
//         }
//     }
//     return true;
// }

// TEST(ConvolveTest, HandlesEmptyImageAndKernel) {
//     Image emptyImage = CREATE_IMG(0, 0, 0, nullptr);
//     Image emptyKernel = CREATE_IMG(0, 0, 0, nullptr);

//     Image result = Convolve(emptyImage, emptyKernel);

//     EXPECT_EQ(result.width, 0);
//     EXPECT_EQ(result.height, 0);
//     EXPECT_EQ(result.channels, 0);
//     EXPECT_EQ(result.image, nullptr);

//     DESTROY_IMG(result);
//     DESTROY_IMG(emptyImage);
//     DESTROY_IMG(emptyKernel);
// }

// TEST(ConvolveTest, SinglePixelIdentityKernel) {
//     unsigned char pixelData[3] = { 100, 150, 200 }; // RGB pixel
//     Image image = CREATE_IMG(1, 1, 3, pixelData);

//     // Identity kernel (1x1 kernel with value 1)
//     unsigned char kernelData[1] = { 1 };
//     Image kernel = CREATE_IMG(1, 1, 1, kernelData);

//     Image result = Convolve(image, kernel);

//     EXPECT_EQ(result.width, 1);
//     EXPECT_EQ(result.height, 1);
//     EXPECT_EQ(result.channels, 3);
//     ASSERT_NE(result.image, nullptr);
//     for (int i = 0; i < 3; ++i) {
//         EXPECT_EQ(result.image[i], pixelData[i]);
//     }

//     DESTROY_IMG(image);
//     DESTROY_IMG(kernel);
//     DESTROY_IMG(result);
// }

// TEST(ConvolveTest, IdentityKernel3x3) {
//     // 3x3 image with 1 channel (grayscale)
//     unsigned char imgData[9] = {
//         10, 20, 30,
//         40, 50, 60,
//         70, 80, 90
//     };
//     Image image = CREATE_IMG(3, 3, 1, imgData);

//     // 3x3 identity kernel (center is 1, others 0)
//     unsigned char kernelData[9] = {
//         0, 0, 0,
//         0, 1, 0,
//         0, 0, 0
//     };
//     Image kernel = CREATE_IMG(3, 3, 1, kernelData);

//     Image result = Convolve(image, kernel);

//     EXPECT_EQ(result.width, 3);
//     EXPECT_EQ(result.height, 3);
//     EXPECT_EQ(result.channels, 1);
//     ASSERT_NE(result.image, nullptr);
//     EXPECT_TRUE(ImagesEqual(image, result));

//     DESTROY_IMG(image);
//     DESTROY_IMG(kernel);
//     DESTROY_IMG(result);
// }

// TEST(ConvolveTest, SimpleBlurKernel) {
//     // 3x3 grayscale image
//     unsigned char imgData[9] = {
//         10, 20, 30,
//         40, 50, 60,
//         70, 80, 90
//     };
//     Image image = CREATE_IMG(3, 3, 1, imgData);

//     // 3x3 averaging kernel (blur)
//     unsigned char kernelData[9] = {
//         1, 1, 1,
//         1, 1, 1,
//         1, 1, 1
//     };
//     Image kernel = CREATE_IMG(3, 3, 1, kernelData);

//     Image result = Convolve(image, kernel);

//     EXPECT_EQ(result.width, 3);
//     EXPECT_EQ(result.height, 3);
//     EXPECT_EQ(result.channels, 1);
//     ASSERT_NE(result.image, nullptr);

//     // Since this is a blur, check that values are within expected range
//     // For example, center pixel should be average of all 9 pixels
//     int sum = 0;
//     for (int i = 0; i < 9; ++i) sum += imgData[i];
//     unsigned char expectedCenter = static_cast<unsigned char>(sum / 9);

//     // Check center pixel (row 1, col 1)
//     int centerIndex = 1 * 3 + 1;
//     EXPECT_NEAR(result.image[centerIndex], expectedCenter, 10); // Allow some tolerance

//     DESTROY_IMG(image);
//     DESTROY_IMG(kernel);
//     DESTROY_IMG(result);
// }

// TEST(ConvolveTest, KernelLargerThanImage) {
//     // 2x2 image, 1 channel
//     unsigned char imgData[4] = { 10, 20, 30, 40 };
//     Image image = CREATE_IMG(2, 2, 1, imgData);

//     // 3x3 kernel
//     unsigned char kernelData[9] = {
//         0, 1, 0,
//         1, 1, 1,
//         0, 1, 0
//     };
//     Image kernel = CREATE_IMG(3, 3, 1, kernelData);

//     Image result = Convolve(image, kernel);

//     EXPECT_EQ(result.width, 2);
//     EXPECT_EQ(result.height, 2);
//     EXPECT_EQ(result.channels, 1);
//     ASSERT_NE(result.image, nullptr);

//     DESTROY_IMG(image);
//     DESTROY_IMG(kernel);
//     DESTROY_IMG(result);
// }

// TEST(ConvolveTest, NonSquareImageAndKernel) {
//     // 3x2 image, 1 channel
//     unsigned char imgData[6] = { 10, 20, 30, 40, 50, 60 };
//     Image image = CREATE_IMG(3, 2, 1, imgData);

//     // 2x3 kernel
//     unsigned char kernelData[6] = {
//         1, 0,
//         0, 1,
//         1, 0
//     };
//     Image kernel = CREATE_IMG(2, 3, 1, kernelData);

//     Image result = Convolve(image, kernel);

//     EXPECT_EQ(result.width, 3);
//     EXPECT_EQ(result.height, 2);
//     EXPECT_EQ(result.channels, 1);
//     ASSERT_NE(result.image, nullptr);

//     DESTROY_IMG(image);
//     DESTROY_IMG(kernel);
//     DESTROY_IMG(result);
// }

// TEST(ConvolveTest, TestInvalidImage) {
//     // We can't actually have an empty array, but
//     // we need to pretend like it is one
//     unsigned char imageData[1] = {0};

//     Image image = {
//         1,
//         -1,
//         0,
//         imageData,
//     };

//     Image kernel = {
//         1,
//         1,
//         1,
//         imageData,
//     };
    

//     ASSERT_ANY_THROW(Convolve(image, kernel));
// }

// Test(ConvolveTest, TestEmptyImage) {
//     // We can't actually have an empty array, but
//     // we need to pretend like it is one
//     unsigned char imageData[1] = {0};   
//     Image image = {
//         0,
//         0,
//         0,
//         imageData,
//     };

//     Image kernel = {
//         1,
//         1,
//         1,
//         imageData,
//     };

//     Image expected = {
//         0,
//         0,
//         0,
//         nullptr
//     }; 

//     Image actual = Convolve(image, kernel);
//     ASSERT_EQ(expected.width, actual.width);
//     ASSERT_EQ(expected.height, actual.height);
//     ASSERT_EQ(expected.channels, actual.channels);
//     ASSERT_EQ(expected.image, actual.image);
//     delete[] actual.image;  // Clean up the allocated memory
// }
