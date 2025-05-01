#include "edge_detection/simple-edge-detection.hpp"
#include <iostream>

/*
 * TestVerticalEdge
 * Test case: Image with a vertical edge in the center.
 * Prints PASSED if edge is detected at the correct location.
 */
void TestVerticalEdge() {
    found::SimpleImage test_img(100, 100);
    // Set right half to white, left half to black
    for (int y = 0; y < 100; ++y) {
        for (int x = 0; x < 100; ++x) {
            test_img.data[y][x] = (x >= 50) ? 255 : 0;
        }
    }
    found::SimpleImage edges = found::SimpleEdgeDetection(test_img, 127);
    bool success = (edges.data[50][50] == 255) && (edges.data[50][51] == 0);
    std::cout << "Vertical Edge Test: " << (success ? "PASSED" : "FAILED") << std::endl;
}

/*
 * TestEmptyImage
 * Test case: Image with zero width and height.
 * Prints PASSED if output image is also empty.
 */
void TestEmptyImage() {
    found::SimpleImage empty_img(0, 0);
    found::SimpleImage edges = found::SimpleEdgeDetection(empty_img, 127);
    bool success = (edges.width == 0 && edges.height == 0);
    std::cout << "Empty Image Test: " << (success ? "PASSED" : "FAILED") << std::endl;
}

int main() {
    TestVerticalEdge();
    TestEmptyImage();
    return 0;
}
