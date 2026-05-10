#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <utility>

#include "src/providers/factory.hpp"
#include "src/providers/stage-providers.hpp"

namespace found {

TEST(StageProvidersTest, TestProvideCalibrationAlgorithmTwice) {
    CalibrationOptions options;

    cnt::unique_ptr<CalibrationAlgorithm> first = ProvideCalibrationAlgorithm(std::move(options));
    ASSERT_NE(nullptr, first.get());
    first.reset();

    CalibrationOptions secondOptions;
    cnt::unique_ptr<CalibrationAlgorithm> second = ProvideCalibrationAlgorithm(std::move(secondOptions));
    ASSERT_NE(nullptr, second.get());
}

TEST(StageProvidersTest, TestProvideEdgeDetectionAlgorithmTwice) {
    DistanceOptions firstOptions;

    cnt::unique_ptr<EdgeDetectionAlgorithm> first = ProvideEdgeDetectionAlgorithm(std::move(firstOptions));
    ASSERT_NE(nullptr, first.get());
    first.reset();

    DistanceOptions secondOptions;
    cnt::unique_ptr<EdgeDetectionAlgorithm> second = ProvideEdgeDetectionAlgorithm(std::move(secondOptions));
    ASSERT_NE(nullptr, second.get());
}

TEST(StageProvidersTest, TestProvideDistanceDeterminationAlgorithmBranches) {
    DistanceOptions sddaOptions;
    sddaOptions.image.width = 2;
    sddaOptions.image.height = 2;
    sddaOptions.distanceAlgo = SDDA;
    cnt::unique_ptr<DistanceDeterminationAlgorithm> sdda =
        ProvideDistanceDeterminationAlgorithm(std::move(sddaOptions));
    ASSERT_NE(nullptr, sdda.get());
    sdda.reset();

    DistanceOptions isddaOptions;
    isddaOptions.image.width = 2;
    isddaOptions.image.height = 2;
    isddaOptions.distanceAlgo = ISDDA;
    cnt::unique_ptr<DistanceDeterminationAlgorithm> isdda =
        ProvideDistanceDeterminationAlgorithm(std::move(isddaOptions));
    ASSERT_NE(nullptr, isdda.get());

    DistanceOptions unknownOptions;
    unknownOptions.image.width = 2;
    unknownOptions.image.height = 2;
    unknownOptions.distanceAlgo = "UNKNOWN";
    ASSERT_THROW(ProvideDistanceDeterminationAlgorithm(std::move(unknownOptions)), std::runtime_error);
}

TEST(StageProvidersTest, TestProvideVectorGenerationAlgorithmBranches) {
    DistanceOptions relativeOptions;

    cnt::unique_ptr<VectorGenerationAlgorithm> fromRelative =
        ProvideVectorGenerationAlgorithm(std::move(relativeOptions));
    ASSERT_NE(nullptr, fromRelative.get());
    fromRelative.reset();

    DistanceOptions referenceOptions;
    referenceOptions.refAsOrientation = true;
    cnt::unique_ptr<VectorGenerationAlgorithm> fromReference =
        ProvideVectorGenerationAlgorithm(std::move(referenceOptions));
    ASSERT_NE(nullptr, fromReference.get());
    fromReference.reset();

    DistanceOptions dataFileOptions;
    dataFileOptions.calibrationData.header = {{'F', 'O', 'U', 'N'}, 1U, 0};
    dataFileOptions.calibrationData.relative_attitude = Quaternion(1, 0, 0, 0);
    cnt::unique_ptr<VectorGenerationAlgorithm> fromDataFile =
        ProvideVectorGenerationAlgorithm(std::move(dataFileOptions));
    ASSERT_NE(nullptr, fromDataFile.get());
}

TEST(FactoryTest, TestCreateCalibrationPipelineExecutorTwice) {
    CalibrationOptions options;

    cnt::unique_ptr<CalibrationPipelineExecutor> first = CreateCalibrationPipelineExecutor(std::move(options));
    ASSERT_NE(nullptr, first.get());
    first.reset();

    CalibrationOptions secondOptions;
    cnt::unique_ptr<CalibrationPipelineExecutor> second = CreateCalibrationPipelineExecutor(std::move(secondOptions));
    ASSERT_NE(nullptr, second.get());
}

TEST(FactoryTest, TestCreateDistancePipelineExecutorTwice) {
    DistanceOptions firstOptions;
    firstOptions.image.width = 2;
    firstOptions.image.height = 2;
    firstOptions.image.channels = 1;
    firstOptions.image.image = nullptr;

    cnt::unique_ptr<DistancePipelineExecutor> first = CreateDistancePipelineExecutor(std::move(firstOptions));
    ASSERT_NE(nullptr, first.get());
    first.reset();

    DistanceOptions secondOptions;
    secondOptions.image.width = 2;
    secondOptions.image.height = 2;
    secondOptions.image.channels = 1;
    secondOptions.image.image = nullptr;

    cnt::unique_ptr<DistancePipelineExecutor> second = CreateDistancePipelineExecutor(std::move(secondOptions));
    ASSERT_NE(nullptr, second.get());
}

}  // namespace found
