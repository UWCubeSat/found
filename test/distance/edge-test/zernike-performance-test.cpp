#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <cstring>

#include "test/common/common.hpp"
#include "src/common/style.hpp"
#include "src/distance/edge.hpp"
#include "src/providers/converters.hpp"

// Forward declaration for stbi_image_free
extern "C" void stbi_image_free(void *retval_from_stbi_load);

// Include stb_image_write for saving images
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "build/libraries/src/stb_image/stb_image_write.h"

namespace found {

// Forward declaration
void generateVisualization(const Image& originalImage, const Points& simplePoints, 
                          const Points& zernikePoints, const std::string& imagePath);

/**
 * Simple performance test for Zernike Edge Detection Algorithm
 * 
 * This test loads an image, runs both SimpleEdgeDetection and ZernikeEdgeDetection,
 * and compares their performance and results.
 */
void runZernikePerformanceTest() {
    std::cout << "========================================\n";
    std::cout << "Zernike Edge Detection Performance Test\n";
    std::cout << "========================================\n\n";

    // Load test image
    std::string imagePath = example_earth1.path;
    std::cout << "Loading image: " << imagePath << "\n";
    
    Image image;
    try {
        image = strtoimage(imagePath);
        std::cout << "Image loaded successfully!\n";
        std::cout << "  Dimensions: " << image.width << " x " << image.height << "\n";
        std::cout << "  Channels: " << image.channels << "\n\n";
    } catch (const std::exception& e) {
        std::cerr << "Error loading image: " << e.what() << "\n";
        return;
    }

    // Create algorithms
    unsigned char threshold = 50;
    int borderLength = 2;
    decimal offset = DECIMAL(0.0);
    
    SimpleEdgeDetectionAlgorithm simpleAlgo(threshold, borderLength, offset);
    auto initialAlgo = std::make_unique<SimpleEdgeDetectionAlgorithm>(threshold, borderLength, offset);
    ZernikeEdgeDetectionAlgorithm zernikeAlgo(
        std::move(initialAlgo),
        7,  // window size
        DECIMAL(1.66)  // transition width
    );

    // Test 1: Simple Edge Detection
    std::cout << "--- Test 1: Simple Edge Detection ---\n";
    auto start = std::chrono::high_resolution_clock::now();
    Points simplePoints = simpleAlgo.Run(image);
    auto end = std::chrono::high_resolution_clock::now();
    auto simpleDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  Points detected: " << simplePoints.size() << "\n";
    std::cout << "  Execution time: " << simpleDuration.count() << " ms\n";
    std::cout << "  Time per point: " << (simplePoints.empty() ? 0.0 : 
        static_cast<double>(simpleDuration.count()) / simplePoints.size()) << " ms/point\n\n";

    // Test 2: Zernike Edge Detection
    std::cout << "--- Test 2: Zernike Edge Detection ---\n";
    start = std::chrono::high_resolution_clock::now();
    Points zernikePoints = zernikeAlgo.Run(image);
    end = std::chrono::high_resolution_clock::now();
    auto zernikeDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  Points detected: " << zernikePoints.size() << "\n";
    std::cout << "  Execution time: " << zernikeDuration.count() << " ms\n";
    std::cout << "  Time per point: " << (zernikePoints.empty() ? 0.0 : 
        static_cast<double>(zernikeDuration.count()) / zernikePoints.size()) << " ms/point\n\n";

    // Comparison
    std::cout << "--- Comparison ---\n";
    std::cout << "  Speedup factor: " << std::fixed << std::setprecision(2)
              << (simpleDuration.count() > 0 ? 
                  static_cast<double>(zernikeDuration.count()) / simpleDuration.count() : 0.0)
              << "x slower\n";
    std::cout << "  Point count difference: " 
              << static_cast<int>(zernikePoints.size()) - static_cast<int>(simplePoints.size()) << "\n\n";

    // Sample points comparison (first 10 points)
    if (!simplePoints.empty() && !zernikePoints.empty()) {
        std::cout << "--- Sample Point Comparison (first 10) ---\n";
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "  Simple Edge Detection:\n";
        size_t sampleSize = std::min(static_cast<size_t>(10), simplePoints.size());
        for (size_t i = 0; i < sampleSize; i++) {
            std::cout << "    Point " << i << ": (" 
                      << simplePoints[i].x << ", " << simplePoints[i].y << ")\n";
        }
        
        std::cout << "  Zernike Edge Detection:\n";
        sampleSize = std::min(static_cast<size_t>(10), zernikePoints.size());
        for (size_t i = 0; i < sampleSize; i++) {
            std::cout << "    Point " << i << ": (" 
                      << zernikePoints[i].x << ", " << zernikePoints[i].y << ")\n";
        }
        std::cout << "\n";

        // Calculate average displacement
        if (simplePoints.size() == zernikePoints.size()) {
            decimal totalDisplacement = DECIMAL(0.0);
            for (size_t i = 0; i < simplePoints.size(); i++) {
                decimal dx = zernikePoints[i].x - simplePoints[i].x;
                decimal dy = zernikePoints[i].y - simplePoints[i].y;
                decimal displacement = DECIMAL_SQRT(dx * dx + dy * dy);
                totalDisplacement += displacement;
            }
            decimal avgDisplacement = totalDisplacement / DECIMAL(simplePoints.size());
            std::cout << "  Average sub-pixel displacement: " << avgDisplacement << " pixels\n";
            std::cout << "  (This shows how much Zernike refined the edge positions)\n\n";
        }
    }

    // Output points to file (optional)
    std::cout << "--- Output Files ---\n";
    std::ofstream simpleFile("simple_edges.txt");
    std::ofstream zernikeFile("zernike_edges.txt");
    
    if (simpleFile.is_open()) {
        simpleFile << std::fixed << std::setprecision(6);
        for (const auto& point : simplePoints) {
            simpleFile << point.x << "\t" << point.y << "\n";
        }
        simpleFile.close();
        std::cout << "  Simple edge points saved to: simple_edges.txt\n";
    }
    
    if (zernikeFile.is_open()) {
        zernikeFile << std::fixed << std::setprecision(6);
        for (const auto& point : zernikePoints) {
            zernikeFile << point.x << "\t" << point.y << "\n";
        }
        zernikeFile.close();
        std::cout << "  Zernike edge points saved to: zernike_edges.txt\n";
    }
    
    std::cout << "\n========================================\n";
    std::cout << "Test completed!\n";
    std::cout << "========================================\n";
    
    // Generate visualization
    std::cout << "\n--- Generating Visualization ---\n";
    generateVisualization(image, simplePoints, zernikePoints, imagePath);
    
    // Free image memory
    if (image.image) {
        stbi_image_free(image.image);
    }
}

namespace {
    /**
     * Draws a point on an image buffer
     */
    void drawPoint(unsigned char* imgData, int width, int height, int channels, 
                   int x, int y, unsigned char r, unsigned char g, unsigned char b, int radius = 2) {
        for (int dy = -radius; dy <= radius; dy++) {
            for (int dx = -radius; dx <= radius; dx++) {
                int px = x + dx;
                int py = y + dy;
                if (px >= 0 && px < width && py >= 0 && py < height) {
                    if (dx*dx + dy*dy <= radius*radius) {
                        int idx = (py * width + px) * channels;
                        imgData[idx] = r;
                        if (channels > 1) imgData[idx + 1] = g;
                        if (channels > 2) imgData[idx + 2] = b;
                    }
                }
            }
        }
    }

    /**
     * Draws a line between two points
     */
    void drawLine(unsigned char* imgData, int width, int height, int channels,
                  int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b) {
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;
        
        int x = x1, y = y1;
        while (true) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                int idx = (y * width + x) * channels;
                imgData[idx] = r;
                if (channels > 1) imgData[idx + 1] = g;
                if (channels > 2) imgData[idx + 2] = b;
            }
            
            if (x == x2 && y == y2) break;
            
            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x += sx;
            }
            if (e2 < dx) {
                err += dx;
                y += sy;
            }
        }
    }

    /**
     * Creates a copy of the image
     */
    unsigned char* copyImage(const Image& img) {
        uint64_t size = static_cast<uint64_t>(img.width * img.height * img.channels);
        unsigned char* copy = new unsigned char[size];
        std::memcpy(copy, img.image, size);
        return copy;
    }
}

void generateVisualization(const Image& originalImage, const Points& simplePoints, 
                          const Points& zernikePoints, const std::string& /* imagePath */) {
    int width = originalImage.width;
    int height = originalImage.height;
    int channels = originalImage.channels;
    
    // Create three visualization images
    unsigned char* img1 = copyImage(originalImage);  // Simple edges
    unsigned char* img2 = copyImage(originalImage);  // Zernike edges
    unsigned char* img3 = copyImage(originalImage);  // Overlay
    
    // Draw simple edges in red
    if (!simplePoints.empty()) {
        Vec2 prevPoint = simplePoints[0];
        for (size_t i = 0; i < simplePoints.size(); i++) {
            int x = static_cast<int>(DECIMAL_ROUND(simplePoints[i].x));
            int y = static_cast<int>(DECIMAL_ROUND(simplePoints[i].y));
            
            drawPoint(img1, width, height, channels, x, y, 255, 0, 0, 2);
            drawPoint(img3, width, height, channels, x, y, 255, 0, 0, 1);
            
            if (i > 0) {
                int px = static_cast<int>(DECIMAL_ROUND(prevPoint.x));
                int py = static_cast<int>(DECIMAL_ROUND(prevPoint.y));
                drawLine(img1, width, height, channels, px, py, x, y, 255, 0, 0);
                drawLine(img3, width, height, channels, px, py, x, y, 255, 0, 0);
            }
            prevPoint = simplePoints[i];
        }
    }
    
    // Draw zernike edges in blue
    if (!zernikePoints.empty()) {
        Vec2 prevPoint = zernikePoints[0];
        for (size_t i = 0; i < zernikePoints.size(); i++) {
            int x = static_cast<int>(DECIMAL_ROUND(zernikePoints[i].x));
            int y = static_cast<int>(DECIMAL_ROUND(zernikePoints[i].y));
            
            drawPoint(img2, width, height, channels, x, y, 0, 0, 255, 2);
            drawPoint(img3, width, height, channels, x, y, 0, 0, 255, 1);
            
            if (i > 0) {
                int px = static_cast<int>(DECIMAL_ROUND(prevPoint.x));
                int py = static_cast<int>(DECIMAL_ROUND(prevPoint.y));
                drawLine(img2, width, height, channels, px, py, x, y, 0, 0, 255);
                drawLine(img3, width, height, channels, px, py, x, y, 0, 0, 255);
            }
            prevPoint = zernikePoints[i];
        }
    }
    
    // Draw displacement arrows in overlay (sample every Nth point)
    if (simplePoints.size() == zernikePoints.size() && !simplePoints.empty()) {
        int sampleRate = std::max(1, static_cast<int>(simplePoints.size()) / 100);
        for (size_t i = 0; i < simplePoints.size(); i += sampleRate) {
            int sx = static_cast<int>(DECIMAL_ROUND(simplePoints[i].x));
            int sy = static_cast<int>(DECIMAL_ROUND(simplePoints[i].y));
            int zx = static_cast<int>(DECIMAL_ROUND(zernikePoints[i].x));
            int zy = static_cast<int>(DECIMAL_ROUND(zernikePoints[i].y));
            
            decimal dx = zernikePoints[i].x - simplePoints[i].x;
            decimal dy = zernikePoints[i].y - simplePoints[i].y;
            decimal dist = DECIMAL_SQRT(dx * dx + dy * dy);
            
            if (dist > DECIMAL(0.5) && sx >= 0 && sx < width && sy >= 0 && sy < height &&
                zx >= 0 && zx < width && zy >= 0 && zy < height) {
                drawLine(img3, width, height, channels, sx, sy, zx, zy, 255, 255, 0);
            }
        }
    }
    
    // Create combined image (3 images side by side)
    int combinedWidth = width * 3;
    int combinedHeight = height;
    unsigned char* combined = new unsigned char[combinedWidth * combinedHeight * channels];
    
    // Copy images side by side
    for (int y = 0; y < height; y++) {
        // Image 1 (Simple)
        std::memcpy(&combined[y * combinedWidth * channels], 
                   &img1[y * width * channels], 
                   width * channels);
        // Image 2 (Zernike)
        std::memcpy(&combined[y * combinedWidth * channels + width * channels], 
                   &img2[y * width * channels], 
                   width * channels);
        // Image 3 (Overlay)
        std::memcpy(&combined[y * combinedWidth * channels + width * 2 * channels], 
                   &img3[y * width * channels], 
                   width * channels);
    }
    
    // Save the combined image
    int result = stbi_write_png("edge_comparison.png", combinedWidth, combinedHeight, channels, 
                                combined, combinedWidth * channels);
    
    if (result) {
        std::cout << "  Visualization saved to: edge_comparison.png\n";
        std::cout << "  Image size: " << combinedWidth << "x" << combinedHeight << "\n";
        std::cout << "  Left: Simple Edge Detection (Red)\n";
        std::cout << "  Middle: Zernike Edge Detection (Blue)\n";
        std::cout << "  Right: Overlay Comparison (Yellow lines show displacement)\n";
    } else {
        std::cout << "  Error: Could not save visualization image\n";
    }
    
    // Cleanup
    delete[] img1;
    delete[] img2;
    delete[] img3;
    delete[] combined;
}

}  // namespace found

// Main function to run the test
int main(int /* argc */, char* /* argv */[]) {
    found::runZernikePerformanceTest();
    return 0;
}

