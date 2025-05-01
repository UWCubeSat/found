#include "edge_detection/simple_threshold_ed.hpp"

SimpleImage simple_threshold_ed(const SimpleImage& input, uint8_t threshold) {
    SimpleImage edges(input.width, input.height);
    for (int y = 1; y < input.height; y++) {
        for (int x = 1; x < input.width; x++) {
            // Apply thresholding to input pixels (convert to binary)
            uint8_t curr = input.data[y][x] > threshold ? 255 : 0;
            uint8_t up   = input.data[y-1][x] > threshold ? 255 : 0;
            uint8_t left = input.data[y][x-1] > threshold ? 255 : 0;
            // Edge if binary value changes from neighbor
            if (curr != up || curr != left) {
                edges.data[y][x] = 255;
            }
        }
    }
    return edges;
}
