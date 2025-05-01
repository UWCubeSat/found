#include "edge_detection/simple-edge-detection.hpp"

namespace found {

SimpleImage::SimpleImage(int width, int height) 
    : width(width), height(height), data(height, std::vector<uint8_t>(width, 0)) {}

SimpleImage SimpleEdgeDetection(const SimpleImage& input_image, uint8_t threshold) {
    SimpleImage edge_image(input_image.width, input_image.height);
    
    for (int y = 1; y < input_image.height; ++y) {
        for (int x = 1; x < input_image.width; ++x) {
            const uint8_t current = input_image.data[y][x] > threshold ? 255 : 0;
            const uint8_t above = input_image.data[y-1][x] > threshold ? 255 : 0;
            const uint8_t left = input_image.data[y][x-1] > threshold ? 255 : 0;
            
            if (current != above || current != left) {
                edge_image.data[y][x] = 255;
            }
        }
    }
    return edge_image;
}

} // namespace found
