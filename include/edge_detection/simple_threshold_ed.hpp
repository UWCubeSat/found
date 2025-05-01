#include <vector>
#include <cstdint>

struct SimpleImage {
    std::vector<std::vector<uint8_t>> data;  // Grayscale (0-255)
    int width;
    int height;
    SimpleImage(int w, int h) : width(w), height(h) {
        data.resize(height, std::vector<uint8_t>(width, 0));
    }
};

SimpleImage simple_threshold_ed(const SimpleImage& input, uint8_t threshold);
