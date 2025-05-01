#ifndef SIMPLE_EDGE_DETECTION_HPP
#define SIMPLE_EDGE_DETECTION_HPP

#include <vector>
#include <cstdint>

namespace found {

/* 
 * SimpleImage represents a grayscale image structure
 * width: Image width in pixels
 * height: Image height in pixels
 * data: 2D vector of grayscale values (0-255)
 */
struct SimpleImage {
    int width;
    int height;
    std::vector<std::vector<uint8_t>> data;
    
    explicit SimpleImage(int width, int height);
};

/*
 * Performs threshold-based edge detection
 * @param input_image: Input grayscale image
 * @param threshold: Binarization threshold (0-255)
 * @return: Image with edges marked as 255 (white)
 */
SimpleImage SimpleEdgeDetection(const SimpleImage& input_image, uint8_t threshold);

} // namespace found
#endif // SIMPLE_EDGE_DETECTION_HPP
