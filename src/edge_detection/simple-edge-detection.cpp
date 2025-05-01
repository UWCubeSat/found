#include "edge_detection/simple-edge-detection.hpp"

/* Implementation of SimpleImage constructor */
found::SimpleImage::SimpleImage(int width, int height)
    : width(width), height(height), data(height, std::vector<uint8_t>(width, 0)) {}

/* Implementation of SimpleEdgeDetection */
found::SimpleImage found::SimpleEdgeDetection(const SimpleImage& input_image, uint8_t threshold) {
    SimpleImage edge_image(input_image.width, input_image.height);

    // Iterate through each pixel, skipping the first row and column (to avoid out-of-bounds)
    for (int y = 1; y y < input_image.height; ++y) {
        for (int x = 1; x < input_image.width; ++x) {
            // Apply threshold to current and neighbor pixels
            uint8_t curr = input_image.data[y][x] > threshold ? 255 : 0;
            uint8_t up = input_image.data[y - 1][x] > threshold ? 255 : 0;
            uint8_t left = input_image.data[y][x - 1] > threshold ? 255 : 0;

            // If there is a difference with top or left neighbor, mark as edge
            if (curr != up || curr != left) {
                edge_image.data[y][x] = 255;
            }
        }
    }
    return edge_image;
}
