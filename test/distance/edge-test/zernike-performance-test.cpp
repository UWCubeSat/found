#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <filesystem>

#include <nlohmann/json.hpp>

#include "test/common/common.hpp"
#include "src/common/style.hpp"
#include "src/distance/edge.hpp"
#include "src/providers/converters.hpp"

// Forward declaration for stbi_image_free
extern "C" void stbi_image_free(void *retval_from_stbi_load);

// Include stb_image_write for saving images (header downloaded by CMake to .cache)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace found {

// Forward declarations for visualizations.
// Writes edge_comparison image next to the input image and returns output path.
std::string generateVisualization(const Image& originalImage, const Points& simplePoints,
                                  const Points& zernikePoints, const std::string& imagePath,
                                  const std::string& outputPathOverride = "");
// Thicker variant used for separate thicker outputs.
std::string generateVisualizationThicker(const Image& originalImage, const Points& simplePoints,
                                         const Points& zernikePoints, const std::string& imagePath,
                                         const std::string& outputPathOverride);

/**
 * Loads initial edge points from a JSON file with format {"points": [[x,y], ...], "count": n}.
 *
 * @param jsonPath Path to the JSON file (e.g. "test/common/assets/chosen_points.json").
 * @param outPoints Output vector of points (Vec2 with decimal x, y).
 * @return true if loaded successfully, false otherwise.
 */
bool loadInitialPointsFromJson(const std::string& jsonPath, Points& outPoints) {
    std::ifstream f(jsonPath);
    if (!f.good()) {
        std::cerr << "Could not open JSON file: " << jsonPath << "\n";
        return false;
    }
    nlohmann::json j;
    try {
        f >> j;
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << "\n";
        return false;
    }
    if (!j.contains("points") || !j["points"].is_array()) {
        std::cerr << "JSON must contain a \"points\" array.\n";
        return false;
    }
    outPoints.clear();
    for (const auto& pt : j["points"]) {
        if (!pt.is_array() || pt.size() < 2) continue;
        outPoints.push_back(Vec2{DECIMAL(pt[0].get<double>()), DECIMAL(pt[1].get<double>())});
    }
    return true;
}

static bool fileExists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

static std::string findImageWithExtensions(const std::string& basePath,
                                          const std::vector<std::string>& extensions) {
    for (const auto& ext : extensions) {
        std::string p = basePath + ext;
        if (fileExists(p)) return p;
    }
    return "";
}

static const std::vector<std::string> kImageExtensions = {".png", ".jpg", ".JPG", ".PNG"};

bool runZernikeForAsset(const std::string& imagePath,
                       const std::string& pointsJsonPath,
                       const std::string& outputPath) {
    std::cout << "  Loading image..." << std::flush;
    Image image;
    try {
        image = strtoimage(imagePath);
    } catch (const std::exception& e) {
        std::cerr << " Error: " << e.what() << "\n";
        return false;
    }
    std::cout << " " << image.width << "x" << image.height << std::endl;

    std::cout << "  Loading points..." << std::flush;
    Points initialPoints;
    if (!loadInitialPointsFromJson(pointsJsonPath, initialPoints)) {
        if (image.image) stbi_image_free(image.image);
        std::cout << " Failed." << std::endl;
        return false;
    }
    std::cout << " " << initialPoints.size() << " points." << std::endl;

    std::cout << "  Running Zernike refinement..." << std::flush;
    auto initialAlgo = std::make_unique<SimpleEdgeDetectionAlgorithm>(50, 2, DECIMAL(0.0));
    ZernikeEdgeDetectionAlgorithm zernikeAlgo(std::move(initialAlgo), 30, DECIMAL(1.66));
    Points zernikePoints = zernikeAlgo.Run(image, initialPoints);
    std::cout << " done." << std::endl;

    Points simplePoints = initialPoints;
    std::cout << "  Writing visualization..." << std::flush;
    std::string out = generateVisualization(image, simplePoints, zernikePoints, imagePath, outputPath);
    if (image.image) stbi_image_free(image.image);
    std::cout << (out.empty() ? " Failed." : " done.") << std::endl;
    return !out.empty();
}

bool runZernikeForAsset(const std::string& imagePath,
                       const std::string& pointsJsonPath,
                       int windowSize,
                       const std::string& outputPath) {
    std::cout << "  Loading image..." << std::flush;
    Image image;
    try {
        image = strtoimage(imagePath);
    } catch (const std::exception& e) {
        std::cerr << " Error: " << e.what() << "\n";
        return false;
    }
    std::cout << " " << image.width << "x" << image.height << std::endl;

    std::cout << "  Loading points..." << std::flush;
    Points initialPoints;
    if (!loadInitialPointsFromJson(pointsJsonPath, initialPoints)) {
        if (image.image) stbi_image_free(image.image);
        std::cout << " Failed." << std::endl;
        return false;
    }
    std::cout << " " << initialPoints.size() << " points." << std::endl;

    std::cout << "  Running Zernike refinement (window size = " << windowSize << ")..." << std::flush;
    auto initialAlgo = std::make_unique<SimpleEdgeDetectionAlgorithm>(50, 2, DECIMAL(0.0));
    ZernikeEdgeDetectionAlgorithm zernikeAlgo(std::move(initialAlgo), windowSize, DECIMAL(1.66));
    Points zernikePoints = zernikeAlgo.Run(image, initialPoints);
    std::cout << " done." << std::endl;

    Points simplePoints = initialPoints;
    std::cout << "  Writing visualization (thicker)..." << std::flush;
    std::string out = generateVisualizationThicker(image, simplePoints, zernikePoints, imagePath, outputPath);
    if (image.image) stbi_image_free(image.image);
    std::cout << (out.empty() ? " Failed." : " done.") << std::endl;
    return !out.empty();
}

void runZernikePerformanceTestAllZernikeAssets() {
    const std::string baseDir = "test/common/assets/zernike/";
    std::cout << "========================================\n";
    std::cout << "Zernike Edge Detection – All Zernike Assets\n";
    std::cout << "========================================\n\n";

    const std::vector<int> windowSizes = {5, 7, 9, 15, 31};

    for (int windowSize : windowSizes) {
        std::string windowDir = baseDir + "thicker/window_" + std::to_string(windowSize) + "/";
        std::filesystem::create_directories(windowDir);
        std::cout << "=== Window size " << windowSize << " ===\n\n";

        // Earth assets 1–12
        for (int n = 1; n <= 12; n++) {
            std::string prefix = baseDir + std::to_string(n) + "_";
            std::string imagePath = findImageWithExtensions(prefix + "example_earth", kImageExtensions);
            std::string pointsPath = prefix + "chosen_points.json";
            if (imagePath.empty() || !fileExists(pointsPath)) continue;
            std::string outputPath = windowDir + std::to_string(n) + "_edge_comparison.jpg";
            std::cout << "--- Asset " << n << " (earth) ---\n"
                      << "  Image: " << imagePath << "\n"
                      << "  Points: " << pointsPath << "\n";
            if (runZernikeForAsset(imagePath, pointsPath, windowSize, outputPath)) {
                std::cout << "  Output: " << outputPath << "\n";
            } else {
                std::cout << "  Failed.\n";
            }
            std::cout << "\n";
        }

        // Moon assets 1–2
        for (int n = 1; n <= 2; n++) {
            std::string prefix = baseDir + std::to_string(n) + "_";
            std::string imagePath = findImageWithExtensions(prefix + "example_moon", kImageExtensions);
            std::string pointsPath = prefix + "moon_chosen_points.json";
            if (imagePath.empty() || !fileExists(pointsPath)) continue;
            std::string outputPath = windowDir + std::to_string(n) + "_moon_edge_comparison.png";
            std::cout << "--- Asset " << n << " (moon) ---\n"
                      << "  Image: " << imagePath << "\n"
                      << "  Points: " << pointsPath << "\n";
            if (runZernikeForAsset(imagePath, pointsPath, windowSize, outputPath)) {
                std::cout << "  Output: " << outputPath << "\n";
            } else {
                std::cout << "  Failed.\n";
            }
            std::cout << std::endl;
        }
    }
}

/**
 * Zernike Edge Detection performance test.
 *
 * Loads an image and initial edge points from test/common/assets/chosen_points.json,
 * runs Zernike refinement on those points, and optionally compares with Simple edge
 * detection or visualizes initial vs refined.
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

    // Load initial edge points from JSON (test/common/assets/chosen_points.json)
    const std::string pointsJsonPath = "test/common/assets/chosen_points.json";
    std::cout << "Loading initial points from: " << pointsJsonPath << "\n";
    Points initialPoints;
    if (!loadInitialPointsFromJson(pointsJsonPath, initialPoints)) {
        std::cerr << "Failed to load initial points; aborting test.\n";
        if (image.image) stbi_image_free(image.image);
        return;
    }
    std::cout << "  Loaded " << initialPoints.size() << " initial points\n\n";

    // Zernike algorithm (initial algorithm unused when Run(image, initialPoints) is called)
    auto initialAlgo = std::make_unique<SimpleEdgeDetectionAlgorithm>(50, 2, DECIMAL(0.0));
    ZernikeEdgeDetectionAlgorithm zernikeAlgo(
        std::move(initialAlgo),
        30,  // window size
        DECIMAL(1.66)  // transition width
    );

    // Run Zernike refinement using JSON initial points
    std::cout << "--- Zernike Edge Detection (refining JSON initial points) ---\n";
    auto start = std::chrono::high_resolution_clock::now();
    Points zernikePoints = zernikeAlgo.Run(image, initialPoints);
    auto end = std::chrono::high_resolution_clock::now();
    auto zernikeDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "  Window size: " << zernikeAlgo.windowSize() << " pixels\n";
    std::cout << "  Points refined: " << zernikePoints.size() << "\n";
    std::cout << "  Execution time: " << zernikeDuration.count() << " ms\n";
    std::cout << "  Time per point: " << (zernikePoints.empty() ? 0.0 :
        static_cast<double>(zernikeDuration.count()) / zernikePoints.size()) << " ms/point\n\n";

    // For visualization: "simple" = initial (from JSON), "zernike" = refined
    Points simplePoints = initialPoints;

    // Comparison
    std::cout << "--- Comparison ---\n";
    std::cout << "  Point count: initial " << simplePoints.size() << " -> refined " << zernikePoints.size() << "\n\n";

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
        std::cout << "  Initial points saved to: simple_edges.txt\n";
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
    
    // Generate visualization (saved next to the input image)
    std::cout << "\n--- Generating Visualization ---\n";
    std::string outputPath = generateVisualization(image, simplePoints, zernikePoints, imagePath);
    if (!outputPath.empty()) {
        std::cout << "  Open this file to view the edge comparison: " << outputPath << "\n";
    }
    
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
     * Draws a line between two points.
     * Skips drawing if either endpoint is out of safe range to avoid huge Bresenham iterations
     * (e.g. from NaN coordinates).
     */
    void drawLine(unsigned char* imgData, int width, int height, int channels,
                  int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b) {
        const int margin = 256;
        if (x1 < -margin || x1 > width + margin || y1 < -margin || y1 > height + margin ||
            x2 < -margin || x2 > width + margin || y2 < -margin || y2 > height + margin) {
            return;
        }
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
     * Draws a thicker line by rendering multiple offset lines around the center line.
     * The effective thickness is approximately `thickness` pixels.
     */
    void drawThickLine(unsigned char* imgData, int width, int height, int channels,
                       int x1, int y1, int x2, int y2,
                       unsigned char r, unsigned char g, unsigned char b,
                       int thickness) {
        int radius = thickness / 2;
        for (int oy = -radius; oy <= radius; ++oy) {
            for (int ox = -radius; ox <= radius; ++ox) {
                if (ox * ox + oy * oy > radius * radius) {
                    continue;
                }
                drawLine(imgData, width, height, channels,
                         x1 + ox, y1 + oy, x2 + ox, y2 + oy, r, g, b);
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

std::string generateVisualization(const Image& originalImage, const Points& simplePoints,
                                  const Points& zernikePoints, const std::string& imagePath,
                                  const std::string& outputPathOverride) {
    if (!originalImage.image) return "";
    int width = originalImage.width;
    int height = originalImage.height;
    int channels = originalImage.channels;
    if (width <= 0 || height <= 0 || channels <= 0) return "";

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

    // On the overlay: draw yellow on top of the blue line where it differs from the red (simple) line
    const decimal diffThreshold = DECIMAL(0.5);
    if (simplePoints.size() == zernikePoints.size() && !zernikePoints.empty()) {
        for (size_t i = 1; i < zernikePoints.size(); i++) {
            decimal d0 = DECIMAL_SQRT(
                (zernikePoints[i - 1].x - simplePoints[i - 1].x) * (zernikePoints[i - 1].x - simplePoints[i - 1].x) +
                (zernikePoints[i - 1].y - simplePoints[i - 1].y) * (zernikePoints[i - 1].y - simplePoints[i - 1].y));
            decimal d1 = DECIMAL_SQRT(
                (zernikePoints[i].x - simplePoints[i].x) * (zernikePoints[i].x - simplePoints[i].x) +
                (zernikePoints[i].y - simplePoints[i].y) * (zernikePoints[i].y - simplePoints[i].y));
            if (d0 > diffThreshold || d1 > diffThreshold) {
                int x0 = static_cast<int>(DECIMAL_ROUND(zernikePoints[i - 1].x));
                int y0 = static_cast<int>(DECIMAL_ROUND(zernikePoints[i - 1].y));
                int x1 = static_cast<int>(DECIMAL_ROUND(zernikePoints[i].x));
                int y1 = static_cast<int>(DECIMAL_ROUND(zernikePoints[i].y));
                drawLine(img3, width, height, channels, x0, y0, x1, y1, 255, 255, 0);
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
    
    // Save the combined image next to the input image (or use outputPathOverride)
    std::string outputPath;
    if (!outputPathOverride.empty()) {
        outputPath = outputPathOverride;
    } else {
        std::string::size_type lastSlash = imagePath.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            outputPath = imagePath.substr(0, lastSlash + 1);
        }
        outputPath += "edge_comparison.png";
    }

    bool isJpg = (outputPath.size() >= 4 &&
                  (outputPath.substr(outputPath.size() - 4) == ".jpg" ||
                   outputPath.substr(outputPath.size() - 4) == ".JPG"));
    int result;
    if (isJpg) {
        result = stbi_write_jpg(outputPath.c_str(), combinedWidth, combinedHeight, channels,
                               combined, 95);
    } else {
        result = stbi_write_png(outputPath.c_str(), combinedWidth, combinedHeight, channels,
                              combined, combinedWidth * channels);
    }

    if (result) {
        std::cout << "  Visualization saved to: " << outputPath << "\n";
        std::cout << "  Image size: " << combinedWidth << "x" << combinedHeight << "\n";
        std::cout << "  Left: Initial points (Red)\n";
        std::cout << "  Middle: Zernike refined (Blue)\n";
        std::cout << "  Right: Red + Blue overlay; Yellow = blue where refined differs from initial\n";
    } else {
        std::cout << "  Error: Could not save visualization image\n";
        outputPath.clear();
    }

    // Cleanup
    delete[] img1;
    delete[] img2;
    delete[] img3;
    delete[] combined;

    return outputPath;
}

// Creates a duplicate visualization with thicker lines (approx. 20 px thickness),
// saving it to the provided outputPathOverride.
std::string generateVisualizationThicker(const Image& originalImage, const Points& simplePoints,
                                         const Points& zernikePoints, const std::string& imagePath,
                                         const std::string& outputPathOverride) {
    if (!originalImage.image) return "";
    int width = originalImage.width;
    int height = originalImage.height;
    int channels = originalImage.channels;
    if (width <= 0 || height <= 0 || channels <= 0) return "";

    // Scale thickness based on image size.
    // Calibrated so that an image with min(width, height) == 3280 (asset 9) uses thickness 20.
    const int minDim = (width < height) ? width : height;
    const double scale = 20.0 / 3280.0;
    int thickness = static_cast<int>(std::lround(scale * static_cast<double>(minDim)));
    if (thickness < 3) thickness = 3;
    if (thickness > 80) thickness = 80;

    unsigned char* img1 = copyImage(originalImage);
    unsigned char* img2 = copyImage(originalImage);
    unsigned char* img3 = copyImage(originalImage);

    // Simple edges in red (points as before, lines thicker)
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
                drawThickLine(img1, width, height, channels, px, py, x, y, 255, 0, 0, thickness);
                drawThickLine(img3, width, height, channels, px, py, x, y, 255, 0, 0, thickness);
            }
            prevPoint = simplePoints[i];
        }
    }

    // Zernike edges in blue
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
                drawThickLine(img2, width, height, channels, px, py, x, y, 0, 0, 255, thickness);
                drawThickLine(img3, width, height, channels, px, py, x, y, 0, 0, 255, thickness);
            }
            prevPoint = zernikePoints[i];
        }
    }

    // Yellow overlay where Zernike differs from simple
    const decimal diffThreshold = DECIMAL(0.5);
    if (simplePoints.size() == zernikePoints.size() && !zernikePoints.empty()) {
        for (size_t i = 1; i < zernikePoints.size(); i++) {
            decimal d0 = DECIMAL_SQRT(
                (zernikePoints[i - 1].x - simplePoints[i - 1].x) * (zernikePoints[i - 1].x - simplePoints[i - 1].x) +
                (zernikePoints[i - 1].y - simplePoints[i - 1].y) * (zernikePoints[i - 1].y - simplePoints[i - 1].y));
            decimal d1 = DECIMAL_SQRT(
                (zernikePoints[i].x - simplePoints[i].x) * (zernikePoints[i].x - simplePoints[i].x) +
                (zernikePoints[i].y - simplePoints[i].y) * (zernikePoints[i].y - simplePoints[i].y));
            if (d0 > diffThreshold || d1 > diffThreshold) {
                int x0 = static_cast<int>(DECIMAL_ROUND(zernikePoints[i - 1].x));
                int y0 = static_cast<int>(DECIMAL_ROUND(zernikePoints[i - 1].y));
                int x1 = static_cast<int>(DECIMAL_ROUND(zernikePoints[i].x));
                int y1 = static_cast<int>(DECIMAL_ROUND(zernikePoints[i].y));
                drawThickLine(img3, width, height, channels, x0, y0, x1, y1, 255, 255, 0, thickness);
            }
        }
    }

    int combinedWidth = width * 3;
    int combinedHeight = height;
    unsigned char* combined = new unsigned char[combinedWidth * combinedHeight * channels];

    for (int y = 0; y < height; y++) {
        std::memcpy(&combined[y * combinedWidth * channels],
                    &img1[y * width * channels],
                    width * channels);
        std::memcpy(&combined[y * combinedWidth * channels + width * channels],
                    &img2[y * width * channels],
                    width * channels);
        std::memcpy(&combined[y * combinedWidth * channels + width * 2 * channels],
                    &img3[y * width * channels],
                    width * channels);
    }

    std::string outputPath;
    if (!outputPathOverride.empty()) {
        outputPath = outputPathOverride;
    } else {
        std::string::size_type lastSlash = imagePath.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            outputPath = imagePath.substr(0, lastSlash + 1);
        }
        outputPath += "edge_comparison_thicker.png";
    }

    bool isJpg = (outputPath.size() >= 4 &&
                  (outputPath.substr(outputPath.size() - 4) == ".jpg" ||
                   outputPath.substr(outputPath.size() - 4) == ".JPG"));
    int result;
    if (isJpg) {
        result = stbi_write_jpg(outputPath.c_str(), combinedWidth, combinedHeight, channels,
                                combined, 95);
    } else {
        result = stbi_write_png(outputPath.c_str(), combinedWidth, combinedHeight, channels,
                                combined, combinedWidth * channels);
    }

    if (!result) {
        outputPath.clear();
    }

    delete[] img1;
    delete[] img2;
    delete[] img3;
    delete[] combined;

    return outputPath;
}

}  // namespace found

// Run with: found-test --gtest_filter=*ZernikePerformance*
TEST(ZernikePerformanceTest, RunZernikePerformanceTest) {
    found::runZernikePerformanceTest();
}

// Run with: found-test --gtest_filter=*AllZernikeAssets*
TEST(ZernikePerformanceTest, RunZernikePerformanceTestAllZernikeAssets) {
    found::runZernikePerformanceTestAllZernikeAssets();
}