#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <utility>
#include <memory>

#include "src/common/spatial/attitude-utils.hpp"
#include "src/common/style.hpp"

#include "test/common/constants/pipeline-constants.hpp"
#include "test/common/mocks/pipeline-mocks.hpp"
#include "test/common/mocks/distance-mocks.hpp"
#include "test/common/common.hpp"

#include "src/common/pipeline/pipelines.hpp"

// This file is dedicated to generating full coverage on pipeline and stage
// objects that resemble the actual pipelines we have.
//
// Note: You must modify this file if the nature of the input and output pipelines
// change

// TODO: Add to this for orbit stages

namespace found {

using Orientations = std::pair<EulerAngles, EulerAngles>;

TEST(NominalPipelineTest, TestPipelinesAsStages) {
    // skip calibration stage, because it only has 1 stage

    DistancePipeline distancePipeline;
    PositionVector expectedVec(1, 2, 3);

    // Here, we make a single stage that looks like the distance pipeline
    std::unique_ptr<MockFunctionStage<Image, PositionVector>>
        distanceStage(new MockFunctionStage<Image, PositionVector>());
    EXPECT_CALL(*distanceStage, Run(testing::_))
        .WillOnce(testing::Return(expectedVec));

    PositionVector actualVec(distancePipeline.Complete(*distanceStage)
                                             .Run({}));

    ASSERT_VEC3_EQ_DEFAULT(expectedVec, actualVec);

    OrbitPipeline orbitPipeline;
    LocationRecords expectedLR;

    // Again, we make a single stage regardless of how many there
    // actually are in the pipeline
    std::unique_ptr<MockFunctionStage<LocationRecords, LocationRecords>>
        orbitStage(new MockFunctionStage<LocationRecords, LocationRecords>());
    EXPECT_CALL(*orbitStage, Run(testing::_))
        .WillOnce(testing::Return(expectedLR));

    orbitPipeline.Complete(*orbitStage);

    OrbitPipeline wrapperOrbitPipeline;
    wrapperOrbitPipeline.Complete(orbitPipeline);
    LocationRecords actualLR(wrapperOrbitPipeline.Run({}));

    ASSERT_THAT(expectedLR, LocationRecordsEqual(actualLR));
}

TEST(NominalPipelineTest, TestNominalPipelinesWrapped) {
    CalibrationPipeline calibrationPipeline;
    Quaternion expectedQuat{1, 2, 3, 4};

    std::unique_ptr<MockFunctionStage<Orientations, Quaternion>>
        calibrationStage(new MockFunctionStage<Orientations, Quaternion>());
    EXPECT_CALL(*calibrationStage, Run(testing::_))
        .WillOnce(testing::Return(expectedQuat));

    calibrationPipeline.Complete(*calibrationStage);

    CalibrationPipeline wrapperCalibrationPipeline;
    wrapperCalibrationPipeline.Complete(calibrationPipeline);
    Quaternion actualQuat(wrapperCalibrationPipeline.Run({}));

    ASSERT_QUAT_EQ_DEFAULT(expectedQuat, actualQuat);

    DistancePipeline distancePipeline;
    PositionVector expectedVec(1, 2, 3);

    std::unique_ptr<MockEdgeDetectionAlgorithm>
        edgeDetectionStage(new MockEdgeDetectionAlgorithm());
    EXPECT_CALL(*edgeDetectionStage, Run(testing::_))
        .WillOnce(testing::Return(Points()));

    std::unique_ptr<MockDistanceDeterminationAlgorithm>
        distanceStage(new MockDistanceDeterminationAlgorithm());
    EXPECT_CALL(*distanceStage, Run(testing::_))
        .WillOnce(testing::Return(PositionVector()));

    std::unique_ptr<MockVectorGenerationAlgorithm>
        vectorStage(new MockVectorGenerationAlgorithm());
    EXPECT_CALL(*vectorStage, Run(testing::_))
        .WillOnce(testing::Return(expectedVec));

    distancePipeline.AddStage(*edgeDetectionStage)
                    .AddStage(*distanceStage)
                    .Complete(*vectorStage);

    DistancePipeline wrapperDistancePipeline;
    wrapperDistancePipeline.Complete(distancePipeline);
    PositionVector actualVec(wrapperDistancePipeline.Run({}));

    ASSERT_VEC3_EQ_DEFAULT(expectedVec, actualVec);

    OrbitPipeline orbitPipeline;
    LocationRecords expectedLR;

    std::unique_ptr<MockFunctionStage<LocationRecords, LocationRecords>>
        undefinedOrbitStage(new MockFunctionStage<LocationRecords, LocationRecords>());
    EXPECT_CALL(*undefinedOrbitStage, Run(testing::_))
        .WillOnce(testing::Return(expectedLR));

    orbitPipeline.Complete(*undefinedOrbitStage);

    OrbitPipeline wrapperOrbitPipeline;
    wrapperOrbitPipeline.Complete(orbitPipeline);
    LocationRecords actualLR(wrapperOrbitPipeline.Run({}));

    ASSERT_THAT(expectedLR, LocationRecordsEqual(actualLR));
}

}  // namespace found
