#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <utility>
#include <string>
#include <sstream>
#include <cstdio>
#include <fstream>
#include <ctime>

#include "test/common/mocks/distance-mocks.hpp"
#include "test/common/mocks/orbit-mocks.hpp"
#include "test/common/common.hpp"

#include "src/providers/converters.hpp"
#include "src/datafile/datafile.hpp"
#include "src/datafile/serialization.hpp"

#include "src/calibrate/calibrate.hpp"

#include "src/distance/edge.hpp"
#include "src/distance/distance.hpp"
#include "src/distance/vectorize.hpp"

// TODO: Fully Implement orbit stuff this after orbit stage is implemented
// TODO: Include statement for Orbit Pipeline

#include "src/command-line/execution/executors.hpp"

namespace found {

MATCHER_P(ImageMatcher, expected, "") {
    return arg.width == expected.width && arg.height == expected.height &&
           arg.channels == expected.channels && arg.image == expected.image;
}

MATCHER_P(PointsMatcher, expected, "") {
    return arg.size() == expected.size() && std::equal(arg.begin(), arg.end(), expected.begin(), expected.end(),
        [](const Vec2& a, const Vec2& b) {
            return a.x == b.x && a.y == b.y;
        });
}

MATCHER_P(PositionVectorMatcher, expected, "") {
    return arg.x == expected.x && arg.y == expected.y && arg.z == expected.z;
}

TEST(ExecutorsTest, TestCalibrationPipelineExecutor) {
    CalibrationOptions options = {
        {DECIMAL_M_PI / 3, 0, 0},
        {DECIMAL_M_PI / 3, -DECIMAL_M_PI / 6, 0},
        temp_df
    };

    CalibrationPipelineExecutor executor(std::move(options), std::make_unique<LOSTCalibrationAlgorithm>());
    executor.ExecutePipeline();

    testing::internal::CaptureStdout();  // Start capturing stdout

    executor.OutputResults();

    std::string output = testing::internal::GetCapturedStdout();  // Stop capturing stdout

    // Both expected outputs are confirmed to conform to expectation
    std::stringstream expectedOutput;
    #ifdef FOUND_FLOAT_MODE
        expectedOutput << "\\[INFO\\s[0-9]{4}-[0-9]{2}-[0-9]{2}\\s[0-9]{2}:[0-9]{2}:[0-9]{2}\\s[A-Z]+\\] "
                    << "Calibration Quaternion: \\((0.965926, 0.224144, -0.12941, 0)\\)\\s*";
    #else
        expectedOutput << "\\[INFO\\s[0-9]{4}-[0-9]{2}-[0-9]{2}\\s[0-9]{2}:[0-9]{2}:[0-9]{2}\\s[A-Z]+\\] "
                    << "Calibration Quaternion: \\((0.965926, 0.224144, -0.12941, -5.55112e-17)\\)\\s*";
    #endif

    ASSERT_THAT(output, testing::MatchesRegex(expectedOutput.str()));

    DataFile expected{
        {},
        LOSTCalibrationAlgorithm().Run({options.lclOrientation, options.refOrientation})
    };

    std::ifstream file(temp_df);
    DataFile actual = deserializeDataFile(file);
    ASSERT_DF_EQ(expected, actual, 1);

    std::remove(temp_df);
}

TEST(ExecutorsTest, TestDistancePipelineExecutor) {
    DistanceOptions options = {
        strtoimage("test/common/assets/example_image.jpg"),
        strtodf("test/common/assets/empty-df.found"),
        false,
        0.012,
        20E-6,
        {0, 0, 0},
        {0, 0, 0},
        DECIMAL_M_E,
        25,
        1,
        0.0,
        "hello",
        92,
        300,
        2.0,
        10,
        12,
        temp_df
    };
    Points points = {
        {0, 0},
        {1, 1},
        {2, 2}
    };
    PositionVector positionVector1{1, 2, 3};
    PositionVector positionVector2{4, 5, 6};

    // Setup Mocks
    std::unique_ptr<MockEdgeDetectionAlgorithm> mockEdgeDetectionAlgorithm =
        std::make_unique<MockEdgeDetectionAlgorithm>();
    EXPECT_CALL(*mockEdgeDetectionAlgorithm, Run(ImageMatcher(options.image)))
        .WillOnce(testing::Return(points));

    std::unique_ptr<MockDistanceDeterminationAlgorithm> mockDistanceDeterminationAlgorithm =
        std::make_unique<MockDistanceDeterminationAlgorithm>();
    EXPECT_CALL(*mockDistanceDeterminationAlgorithm, Run(PointsMatcher(points)))
        .WillOnce(testing::Return(positionVector1));

    std::unique_ptr<MockVectorGenerationAlgorithm> mockVectorGenerationAlgorithm =
        std::make_unique<MockVectorGenerationAlgorithm>();
    EXPECT_CALL(*mockVectorGenerationAlgorithm, Run(PositionVectorMatcher(positionVector1)))
        .WillOnce(testing::Return(positionVector2));

    std::unique_ptr<EdgeDetectionAlgorithm>
        edgeDetectionAlgorithm(std::move(mockEdgeDetectionAlgorithm));
    std::unique_ptr<DistanceDeterminationAlgorithm>
        distanceDeterminationAlgorithm(std::move(mockDistanceDeterminationAlgorithm));
    std::unique_ptr<VectorGenerationAlgorithm>
        vectorGenerationAlgorithm(std::move(mockVectorGenerationAlgorithm));

    DistancePipelineExecutor executor(std::move(options),
                                      std::move(edgeDetectionAlgorithm),
                                      std::move(distanceDeterminationAlgorithm),
                                      std::move(vectorGenerationAlgorithm));

    executor.ExecutePipeline();

    testing::internal::CaptureStdout();  // Start capturing stdout

    executor.OutputResults();

    std::string output = testing::internal::GetCapturedStdout();  // Stop capturing stdout

    std::stringstream expectedOutput;
    expectedOutput << "\\[INFO\\s[0-9]{4}-[0-9]{2}-[0-9]{2}\\s[0-9]{2}:[0-9]{2}:[0-9]{2}\\s[A-Z]+\\] "
                   << "Calculated Position: \\(4, 5, 6\\) m\\s*"
                   << "\\[INFO\\s[0-9]{4}-[0-9]{2}-[0-9]{2}\\s[0-9]{2}:[0-9]{2}:[0-9]{2}\\s[A-Z]+\\] "
                   << "Distance from Earth: 8.77496 m\\s*";

    ASSERT_THAT(output, testing::MatchesRegex(expectedOutput.str()));

    DataFile expected{
        {{'F', 'O', 'U', 'N'}, 1U, 1},
        {},
        std::make_unique<LocationRecord[]>(1)
    };
    expected.positions[0] = {145295, {4, 5, 6}};

    std::ifstream file(temp_df);
    DataFile actual = deserializeDataFile(file);
    ASSERT_DF_EQ(expected, actual, 1);

    std::remove(temp_df);
}

TEST(ExecutorsTest, TestOrbitPipelineExecutor) {
    OrbitOptions options = {
        {{1, {1, 1, 1}}, {2, {2, 2, 2}}, {3, {3, 3, 3}}},
        "example.orbit",
        1000,
        0.1,
        0.01,
        0.001
    };

    LocationRecords expectedResult = {
        {4, {4, 4, 4}},
        {5, {5, 5, 5}},
        {6, {6, 6, 6}}
    };

    std::unique_ptr<MockOrbitPropagationAlgorithm> mockOrbitPropagationAlgorithm =
        std::make_unique<MockOrbitPropagationAlgorithm>();
    EXPECT_CALL(*mockOrbitPropagationAlgorithm, Run(testing::_))
        .WillOnce(testing::Return(expectedResult));

    OrbitPipelineExecutor executor(std::move(options), std::move(mockOrbitPropagationAlgorithm));
    executor.ExecutePipeline();

    testing::internal::CaptureStdout();  // Start capturing stdout

    executor.OutputResults();

    std::string output = testing::internal::GetCapturedStdout();  // Stop capturing stdout

    std::stringstream expectedOutput;
    expectedOutput << "\\[INFO\\s[0-9]{4}-[0-9]{2}-[0-9]{2}\\s[0-9]{2}:[0-9]{2}:[0-9]{2}\\s[A-Z]+\\] "
                   << "Calculated Future Position: \\(6, 6, 6\\) m at time 6 s\\s*";

    ASSERT_THAT(output, testing::MatchesRegex(expectedOutput.str()));
}


}  // namespace found
