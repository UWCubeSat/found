#include "edge_detection/simple_threshold_ed.hpp"
#include <iostream>

void test_vertical_edge() {
    SimpleImage test_img(100, 100);
    // Create vertical edge at x=50
    for (int y = 0; y < 100; y++) {
        for (int x = 0; x < 100; x++) {
            test_img.data[y][x] = (x >= 50) ? 255 : 0;
        }
    }
    SimpleImage edges = simple_threshold_ed(test_img, 127);
    bool success = (edges.data[50][50] == 255) && (edges.data[50][51] == 0);
    std::cout << "Vertical Edge Test: " << (success ? "PASSED" : "FAILED") << "\n";
}

void test_empty_image() {
    SimpleImage empty_img(0, 0);
    SimpleImage edges = simple_threshold_ed(empty_img, 127);
    bool success = (edges.width == 0 && edges.height == 0);
    std::cout << "Empty Image Test: " << (success ? "PASSED" : "FAILED") << "\n";
}

int main() {
    test_vertical_edge();
    test_empty_image();
    return 0;
}
