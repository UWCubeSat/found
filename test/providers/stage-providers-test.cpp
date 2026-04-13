#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <utility>

#include "src/providers/factory.hpp"
#include "src/providers/stage-providers.hpp"

namespace found {

TEST(StageProvidersTest, TestProvideCalibrationAlgorithmTwice) {
    CalibrationOptions options;

    unique_ptr<LOSTCalibrationAlgorithm, 1> first = ProvideCalibrationAlgorithm(options);
    ASSERT_NE(nullptr, first.get());
    first.reset();

    unique_ptr<LOSTCalibrationAlgorithm, 1> second = ProvideCalibrationAlgorithm(options);
    ASSERT_NE(nullptr, second.get());
}

TEST(StageProvidersTest, TestProvideEdgeDetectionAlgorithmTwice) {
    DistanceOptions firstOptions;

    unique_ptr<SimpleEdgeDetectionAlgorithm, 1> first = ProvideEdgeDetectionAlgorithm(std::move(firstOptions));
    ASSERT_NE(nullptr, first.get());
    first.reset();

    DistanceOptions secondOptions;
    unique_ptr<SimpleEdgeDetectionAlgorithm, 1> second = ProvideEdgeDetectionAlgorithm(std::move(secondOptions));
    ASSERT_NE(nullptr, second.get());
}

TEST(StageProvidersTest, TestProvideDistanceDeterminationAlgorithmBranches) {
    DistanceOptions sddaOptions;
    sddaOptions.image.width = 2;
    sddaOptions.image.height = 2;
    sddaOptions.distanceAlgo = SDDA;
    unique_ptr<SphericalDistanceDeterminationAlgorithm, 1> sdda =
        ProvideDistanceDeterminationAlgorithm(std::move(sddaOptions));
    ASSERT_NE(nullptr, sdda.get());
    sdda.reset();

    DistanceOptions isddaOptions;
    isddaOptions.image.width = 2;
    isddaOptions.image.height = 2;
    isddaOptions.distanceAlgo = ISDDA;
    unique_ptr<SphericalDistanceDeterminationAlgorithm, 1> isdda =
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

    unique_ptr<LOSTVectorGenerationAlgorithm, 1> fromRelative =
        ProvideVectorGenerationAlgorithm(std::move(relativeOptions));
    ASSERT_NE(nullptr, fromRelative.get());
    fromRelative.reset();

    DistanceOptions referenceOptions;
    referenceOptions.refAsOrientation = true;
    unique_ptr<LOSTVectorGenerationAlgorithm, 1> fromReference =
        ProvideVectorGenerationAlgorithm(std::move(referenceOptions));
    ASSERT_NE(nullptr, fromReference.get());
    fromReference.reset();

    DistanceOptions dataFileOptions;
    dataFileOptions.calibrationData.header = {{'F', 'O', 'U', 'N'}, 1U, 0};
    dataFileOptions.calibrationData.relative_attitude = Quaternion(1, 0, 0, 0);
    unique_ptr<LOSTVectorGenerationAlgorithm, 1> fromDataFile =
        ProvideVectorGenerationAlgorithm(std::move(dataFileOptions));
    ASSERT_NE(nullptr, fromDataFile.get());
}

TEST(FactoryTest, TestCreateCalibrationPipelineExecutorTwice) {
    CalibrationOptions options;

    CalibrationPipelineExecutorPtr first = CreateCalibrationPipelineExecutor(options);
    ASSERT_NE(nullptr, first.get());
    first.reset();

    CalibrationPipelineExecutorPtr second = CreateCalibrationPipelineExecutor(options);
    ASSERT_NE(nullptr, second.get());
}

TEST(FactoryTest, TestCreateDistancePipelineExecutorTwice) {
    DistanceOptions firstOptions;
    firstOptions.image.width = 2;
    firstOptions.image.height = 2;
    firstOptions.image.channels = 1;
    firstOptions.image.image = nullptr;

    DistancePipelineExecutorPtr first = CreateDistancePipelineExecutor(std::move(firstOptions));
    ASSERT_NE(nullptr, first.get());
    first.reset();

    DistanceOptions secondOptions;
    secondOptions.image.width = 2;
    secondOptions.image.height = 2;
    secondOptions.image.channels = 1;
    secondOptions.image.image = nullptr;

    DistancePipelineExecutorPtr second = CreateDistancePipelineExecutor(std::move(secondOptions));
    ASSERT_NE(nullptr, second.get());
}

}  // namespace found
