#ifndef SIMPLE_EDGE_DETECTION_HPP
#define SIMPLE_EDGE_DETECTION_HPP

#include <vector>
#include <cstdint>

/* 
 * SimpleImage
 * Represents a 2D grayscale image for edge detection.
 */
namespace found {

struct SimpleImage {
    int width;
    int height;
    std::vector<std::vector<uint8_t>> data;

    /* 
     * SimpleImage Constructor
     * Initializes an image with the given width and height, all pixels set to 0.
     */
    explicit SimpleImage(int width, int height);
};

/*
 * SimpleEdgeDetection
 * Performs threshold-based edge detection on a grayscale image.
 * Parameters:
 *   input_image: The input grayscale SimpleImage.
 *   threshold:   The threshold value (0-255) for binarization.
 * Returns:
 *   A new SimpleImage containing detected edges (255=white edge, 0=background).
 */
SimpleImage SimpleEdgeDetection(const SimpleImage& input_image, uint8_t threshold);

} // namespace found

#endif // SIMPLE_EDGE_DETECTION_HPP
