#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

#include <nlohmann/json.hpp>

#include "test/common/common.hpp"
#include "src/common/style.hpp"
#include "src/distance/edge.hpp"
#include "src/providers/converters.hpp"

extern "C" void stbi_image_free(void *retval_from_stbi_load);

namespace found {

// Reuse helpers defined in zernike-performance-test.cpp
bool loadInitialPointsFromJson(const std::string& jsonPath, Points& outPoints);

std::string generateVisualization(const Image& originalImage, const Points& simplePoints,
                                  const Points& zernikePoints, const std::string& imagePath,
                                  const std::string& outputPathOverride = "");

std::string generateVisualizationThicker(const Image& originalImage, const Points& simplePoints,
                                         const Points& zernikePoints, const std::string& imagePath,
                                         const std::string& outputPathOverride);

/**
 * Runs Zernike refinement for a single asset and writes both normal and thicker visualizations.
 */
bool runZernikeForSingleAsset(const std::string& imagePath,
                              const std::string& pointsJsonPath,
                              int windowSize,
                              const std::string& normalOutputPath,
                              const std::string& thickerOutputPath) {
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

    std::cout << "  Writing normal visualization..." << std::flush;
    std::string outNormal = generateVisualization(image, simplePoints, zernikePoints, imagePath, normalOutputPath);
    std::cout << (outNormal.empty() ? " Failed." : " done.") << std::endl;

    std::cout << "  Writing thicker visualization..." << std::flush;
    std::string outThicker = generateVisualizationThicker(image, simplePoints, zernikePoints, imagePath, thickerOutputPath);
    std::cout << (outThicker.empty() ? " Failed." : " done.") << std::endl;

    if (image.image) stbi_image_free(image.image);

    return !outNormal.empty() && !outThicker.empty();
}

}  // namespace found

// Run with: found-test --gtest_filter=ZernikeSingleAssetTest.*
TEST(ZernikeSingleAssetTest, RunOnExampleEarth1NormalAndThicker) {
    using namespace found;

    const std::string baseDir = "test/common/assets/zernike/";
    const std::string imagePath = baseDir + "1_example_earth.png";
    const std::string pointsPath = baseDir + "1_chosen_points.json";

    ASSERT_TRUE(std::filesystem::exists(imagePath)) << "Missing image: " << imagePath;
    ASSERT_TRUE(std::filesystem::exists(pointsPath)) << "Missing points JSON: " << pointsPath;

    const std::vector<int> windowSizes = {5, 7, 9, 15, 31};

    for (int windowSize : windowSizes) {
        std::cout << "=== Single asset (1_example_earth) – window size " << windowSize << " ===\n";

        std::string normalDir = baseDir + "window_" + std::to_string(windowSize) + "/";
        std::string thickerDir = baseDir + "thicker/window_" + std::to_string(windowSize) + "/";
        std::filesystem::create_directories(normalDir);
        std::filesystem::create_directories(thickerDir);

        std::string normalOutputPath = normalDir + "1_edge_comparison.jpg";
        std::string thickerOutputPath = thickerDir + "1_edge_comparison.jpg";

        EXPECT_TRUE(runZernikeForSingleAsset(
            imagePath, pointsPath, windowSize, normalOutputPath, thickerOutputPath));
    }
}

TEST(ZernikeSingleAssetTest, RunOnExampleEarth3NormalAndThicker) {
    using namespace found;

    const std::string baseDir = "test/common/assets/zernike/";
    const std::string imagePath = baseDir + "3_example_earth.jpg";
    const std::string pointsPath = baseDir + "3_chosen_points.json";

    ASSERT_TRUE(std::filesystem::exists(imagePath)) << "Missing image: " << imagePath;
    ASSERT_TRUE(std::filesystem::exists(pointsPath)) << "Missing points JSON: " << pointsPath;

    const std::vector<int> windowSizes = {5, 7, 9, 15, 31};

    for (int windowSize : windowSizes) {
        std::cout << "=== Single asset (3_example_earth) – window size " << windowSize << " ===\n";

        std::string normalDir = baseDir + "window_" + std::to_string(windowSize) + "/";
        std::string thickerDir = baseDir + "thicker/window_" + std::to_string(windowSize) + "/";
        std::filesystem::create_directories(normalDir);
        std::filesystem::create_directories(thickerDir);

        std::string normalOutputPath = normalDir + "3_edge_comparison.jpg";
        std::string thickerOutputPath = thickerDir + "3_edge_comparison.jpg";

        EXPECT_TRUE(runZernikeForSingleAsset(
            imagePath, pointsPath, windowSize, normalOutputPath, thickerOutputPath));
    }
}


