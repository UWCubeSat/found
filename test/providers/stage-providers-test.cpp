#include <gtest/gtest.h>

#include <stdexcept>
#include <utility>

#include "src/providers/factory.hpp"
#include "src/providers/stage-providers.hpp"

namespace found {

TEST(StageProvidersTest, TestProvideCalibrationAlgorithmTwice) {
    CalibrationOptions options;

    auto first = ProvideCalibrationAlgorithm(options);
    ASSERT_NE(nullptr, first.get());
    first.reset();

    auto second = ProvideCalibrationAlgorithm(options);
    ASSERT_NE(nullptr, second.get());
}

TEST(StageProvidersTest, TestProvideEdgeDetectionAlgorithmTwice) {
    DistanceOptions options;

    auto first = ProvideEdgeDetectionAlgorithm(options);
    ASSERT_NE(nullptr, first.get());
    first.reset();

    auto second = ProvideEdgeDetectionAlgorithm(options);
    ASSERT_NE(nullptr, second.get());
}

TEST(StageProvidersTest, TestProvideDistanceDeterminationAlgorithmBranches) {
    DistanceOptions options;
    options.image.width = 2;
    options.image.height = 2;

    options.distanceAlgo = SDDA;
    auto sdda = ProvideDistanceDeterminationAlgorithm(options);
    ASSERT_NE(nullptr, sdda.get());
    sdda.reset();

    options.distanceAlgo = ISDDA;
    auto isdda = ProvideDistanceDeterminationAlgorithm(options);
    ASSERT_NE(nullptr, isdda.get());

    options.distanceAlgo = "UNKNOWN";
    ASSERT_THROW(ProvideDistanceDeterminationAlgorithm(options), std::runtime_error);
}

TEST(StageProvidersTest, TestProvideVectorGenerationAlgorithmBranches) {
    DistanceOptions options;

    auto fromRelative = ProvideVectorGenerationAlgorithm(options);
    ASSERT_NE(nullptr, fromRelative.get());
    fromRelative.reset();

    options.refAsOrientation = true;
    auto fromReference = ProvideVectorGenerationAlgorithm(options);
    ASSERT_NE(nullptr, fromReference.get());
    fromReference.reset();

    options.calibrationData.header = {{'F', 'O', 'U', 'N'}, 1U, 0};
    options.calibrationData.relative_attitude = Quaternion(1, 0, 0, 0);
    auto fromDataFile = ProvideVectorGenerationAlgorithm(options);
    ASSERT_NE(nullptr, fromDataFile.get());
}

TEST(FactoryTest, TestCreateCalibrationPipelineExecutorTwice) {
    CalibrationOptions options;

    auto first = CreateCalibrationPipelineExecutor(options);
    ASSERT_NE(nullptr, first.get());
    first.reset();

    auto second = CreateCalibrationPipelineExecutor(options);
    ASSERT_NE(nullptr, second.get());
}

TEST(FactoryTest, TestCreateDistancePipelineExecutorTwice) {
    DistanceOptions firstOptions;
    firstOptions.image.width = 2;
    firstOptions.image.height = 2;
    firstOptions.image.channels = 1;
    firstOptions.image.image = nullptr;

    auto first = CreateDistancePipelineExecutor(std::move(firstOptions));
    ASSERT_NE(nullptr, first.get());
    first.reset();

    DistanceOptions secondOptions;
    secondOptions.image.width = 2;
    secondOptions.image.height = 2;
    secondOptions.image.channels = 1;
    secondOptions.image.image = nullptr;

    auto second = CreateDistancePipelineExecutor(std::move(secondOptions));
    ASSERT_NE(nullptr, second.get());
}

}  // namespace found
