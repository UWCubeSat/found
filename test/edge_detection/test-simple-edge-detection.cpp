#include "edge_detection/simple-edge-detection.hpp"
#include <iostream>

void TestVerticalEdge() {
    found::SimpleImage test_img(100, 100);
    for (int y = 0; y < 100; ++y) {
        for (int x = 0; x < 100; ++x) {
            test_img.data[y][x] = (x >= 50) ? 255 : 0;
        }
    }
    
    const found::SimpleImage edges = found::SimpleEdgeDetection(test_img, 127);
    const bool success = (edges.data[50][50] == 255) && (edges.data[50][51] == 0);
    std::cout << "Vertical Edge Test: " << (success ? "PASSED" : "FAILED") << "\n";
}

int main() {
    TestVerticalEdge();
    return 0;
}
