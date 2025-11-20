#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>

#include "src/common/style.hpp"
#include "src/distance/edge.hpp"

#include "test/common/constants/pipeline-constants.hpp"
#include "test/common/mocks/pipeline-mocks.hpp"
#include "test/common/mocks/distance-mocks.hpp"
#include "test/common/common.hpp"

#include "src/common/pipeline/pipelines.hpp"

namespace found {

using Orientations = std::pair<EulerAngles, EulerAngles>;

/////////////////////////////////////////////
///////// SEQUENTIAL PIPELINE TEST //////////
/////////////////////////////////////////////

/**
 * Tets to running a pipeline with no stages
 */
TEST(SequentialPipelineTest, TestSequentialPipelineRunOnEmpty) {
    INIT_SQ_INT_TO_CHAR_PIPELINE(pipeline);
    int test_input = 2;

    // no stages == fail
    ASSERT_THROW(pipeline.Run(test_input), std::runtime_error);
}

/**
 * Tests completing a pipeline, and then adding stages
 */
TEST(SequentialPipelineTest, TestSequentialPipelineAddStageAfterComplete) {
    INIT_SQ_CHAR_TO_DOUBLE_PIPELINE(pipeline);

    std::unique_ptr<MockFunctionStage<char, double>> stage1(new MockFunctionStage<char, double>());

    std::unique_ptr<MockFunctionStage<int, double>> stage2(new MockFunctionStage<int, double>());

    pipeline.Complete(std::move(stage1));
    ASSERT_THROW(pipeline.AddStage(std::move(stage2)), std::invalid_argument);
}

/**
 * Tests compelteing a pipeline after completing it
 */
TEST(SequentialPipelineTest, TestSequentialPipelineCompleteAfterComplete) {
    INIT_SQ_CHAR_TO_DOUBLE_PIPELINE(pipeline);

    std::unique_ptr<MockFunctionStage<char, double>> stage1(new MockFunctionStage<char, double>());

    std::unique_ptr<MockFunctionStage<int, double>> stage2(new MockFunctionStage<int, double>());

    pipeline.Complete(std::move(stage1));
    ASSERT_THROW(pipeline.Complete(std::move(stage2)), std::invalid_argument);
}

/**
 * Tests adding an invalid first stage
 */
TEST(SequentialPipelineTest, TestSequentialPipelineInvalidFirstStage) {
    INIT_SQ_CHAR_TO_DOUBLE_PIPELINE(pipeline);

    std::unique_ptr<MockFunctionStage<double, int>> stage1(new MockFunctionStage<double, int>());

    ASSERT_THROW(pipeline.AddStage(std::move(stage1)), std::invalid_argument);
}

/**
 * Tests a pipeline with 1 stage
 */
TEST(SequentialPipelineTest, TestSequentialPipelineSingleStage) {
    INIT_SQ_INT_TO_CHAR_PIPELINE(pipeline);

    int test_set = 1;

    std::unique_ptr<MockFunctionStage<int, char>> stage1(new MockFunctionStage<int, char>());
    EXPECT_CALL(*stage1, Run(integers[test_set]))
        .WillOnce(testing::Return(characters[test_set]));

    char result = pipeline.Complete(std::move(stage1))
                          .Run(integers[test_set]);

    ASSERT_EQ(characters[test_set], result);
}

/**
 * Tests a pipeline with 2 stages
 */
TEST(SequentialPipelineTest, TestSequentialPipelineTwoStage) {
    // Make our pipeline and inject our object
    INIT_SQ_INT_TO_CHAR_PIPELINE(pipeline);

    // Define our selected test parameters (from our constants
    // file)
    int test_set = 0;

    // Make our mocks and register it so that
    // it returns what we want. Our mocks are Stage
    // objects, and we've created a Mock Stage (MockFunctionStage)
    // that you can use to mock a stage (so you don't have
    // to write one out). See test/common/mocks/pipeline-mocks.hpp
    // for more information

    // The first stage goes from an integer to a string
    std::unique_ptr<MockFunctionStage<int, std::string>> stage1(new MockFunctionStage<int, std::string>());
    EXPECT_CALL(*stage1, Run(integers[test_set]))
        .WillOnce(testing::Return(strings[test_set]));
    // This line (.WillOnce(...);), we have here to make sure that this function is only called once

    // The second stage takes a string and returns
    // a char
    std::unique_ptr<MockFunctionStage<std::string, char>> stage2(new MockFunctionStage<std::string, char>());
    EXPECT_CALL(*stage2, Run(strings[test_set]))
        .WillOnce(testing::Return(characters[test_set]));

    // Now, we construct the pipeline and run it
    char result = pipeline.AddStage(std::move(stage1))
                          .Complete(std::move(stage2))
                          .Run(integers[test_set]);
    // And we verify the result
    ASSERT_EQ(characters[test_set], result);
}

/**
 * Tests a different pipeline with 2 stages
 */
TEST(SequentialPipelineTest, TestSequentialPipelineTwoStageOther) {
    INIT_SQ_DOUBLE_TO_STRING_PIPELINE(pipeline);

    int test_set = 1;

    std::unique_ptr<MockFunctionStage<double, float>> stage1(new MockFunctionStage<double, float>());
    EXPECT_CALL(*stage1, Run(doubles[test_set]))
        .WillOnce(testing::Return(floats[test_set]));

    std::unique_ptr<MockFunctionStage<float, std::string>> stage2(new MockFunctionStage<float, std::string>());
    EXPECT_CALL(*stage2, Run(floats[test_set]))
        .WillOnce(testing::Return(strings[test_set]));

    std::string result = pipeline.AddStage(std::move(stage1))
                          .Complete(std::move(stage2))
                          .Run(doubles[test_set]);
    ASSERT_EQ(std::string(strings[test_set]), result);
}

/**
 * Tests a pipeline with 3 parameters
 */
TEST(SequentialPipelineTest, TestSequentialPipelineThreeStage) {
    INIT_SQ_CHAR_TO_DOUBLE_PIPELINE(pipeline);

    int test_set = 2;

    std::unique_ptr<MockFunctionStage<char, int>> stage1(new MockFunctionStage<char, int>());
    EXPECT_CALL(*stage1, Run(characters[test_set]))
        .WillOnce(testing::Return(integers[test_set]));

    std::unique_ptr<MockFunctionStage<int, std::string>> stage2(new MockFunctionStage<int, std::string>());
    EXPECT_CALL(*stage2, Run(integers[test_set]))
        .WillOnce(testing::Return(strings[test_set]));

    std::unique_ptr<MockFunctionStage<std::string, double>> stage3(new MockFunctionStage<std::string, double>());
    EXPECT_CALL(*stage3, Run(strings[test_set]))
        .WillOnce(testing::Return(doubles[test_set]));

    double result = pipeline.AddStage(std::move(stage1))
                          .AddStage(std::move(stage2))
                          .Complete(std::move(stage3))
                          .Run(characters[test_set]);
    ASSERT_EQ(doubles[test_set], result);
}

TEST(SequentialPipelineTest, TestSequentialPipelineGeneral) {
    INIT_SQ_DOUBLE_TO_STRING_PIPELINE(pipeline);

    int test_set = 0;

    std::unique_ptr<MockFunctionStage<double, int>> stage1(new MockFunctionStage<double, int>());
    EXPECT_CALL(*stage1, Run(doubles[test_set]))
        .WillOnce(testing::Return(integers[test_set]));

    std::unique_ptr<MockFunctionStage<int, char>> stage2(new MockFunctionStage<int, char>());
    EXPECT_CALL(*stage2, Run(integers[test_set]))
        .WillOnce(testing::Return(characters[test_set]));

    std::unique_ptr<MockFunctionStage<char, double>> stage3(new MockFunctionStage<char, double>());
    EXPECT_CALL(*stage3, Run(characters[test_set]))
        .WillOnce(testing::Return(doubles[test_set]));

    std::unique_ptr<MockFunctionStage<double, std::string>> stage4(new MockFunctionStage<double, std::string>());
    EXPECT_CALL(*stage4, Run(doubles[test_set]))
        .WillOnce(testing::Return(strings[test_set]));

    std::string result = pipeline.AddStage(std::move(stage1))
                                 .AddStage(std::move(stage2))
                                 .AddStage(std::move(stage3))
                                 .Complete(std::move(stage4))
                                 .Run(doubles[test_set]);
    ASSERT_EQ(strings[test_set], result);
}

/**
 * Tests putting a pipline in the beginning of another pipeline
 */
TEST(SequentialPipelineTest, TestSequentialPipelineBeginningPipelineInPipeline) {
    INIT_SQ_CHAR_TO_DOUBLE_PIPELINE(outerPipeline);

    int test_set = 0;

    std::unique_ptr<SequentialPipeline<char, int>> innerPipeline(new SequentialPipeline<char, int>());

    std::unique_ptr<MockFunctionStage<char, char>> innerStage1(new MockFunctionStage<char, char>());
    EXPECT_CALL(*innerStage1, Run(characters[test_set]))
        .WillOnce(testing::Return(characters[test_set]));

    std::unique_ptr<MockFunctionStage<char, int>> innerStage2(new MockFunctionStage<char, int>());
    EXPECT_CALL(*innerStage2, Run(characters[test_set]))
        .WillOnce(testing::Return(integers[test_set]));

    innerPipeline->AddStage(std::move(innerStage1)).Complete(std::move(innerStage2));

    std::unique_ptr<MockFunctionStage<int, int>> outerStage1(new MockFunctionStage<int, int>());
    EXPECT_CALL(*outerStage1, Run(integers[test_set]))
        .WillOnce(testing::Return(integers[test_set]));

    std::unique_ptr<MockFunctionStage<int, double>> outerStage2(new MockFunctionStage<int, double>());
    EXPECT_CALL(*outerStage2, Run(integers[test_set]))
        .WillOnce(testing::Return(doubles[test_set]));

    outerPipeline.AddStage(std::move(innerPipeline))
                 .AddStage(std::move(outerStage1))
                 .Complete(std::move(outerStage2));

    double result = outerPipeline.Run(characters[test_set]);

    ASSERT_EQ(result, doubles[test_set]);
}

/**
 * Tests adding a Pipeline into the middle of another Pipeline
 */
TEST(SequentialPipelineTest, TestSequentialPipelineMiddlePipelineInPipeline) {
    INIT_SQ_CHAR_TO_DOUBLE_PIPELINE(outerPipeline);

    int test_set = 2;

    std::unique_ptr<MockFunctionStage<char, int>> outerStage1(new MockFunctionStage<char, int>());
    EXPECT_CALL(*outerStage1, Run(characters[test_set]))
        .WillOnce(testing::Return(integers[test_set]));

    std::unique_ptr<SequentialPipeline<int, float>> innerPipeline(new SequentialPipeline<int, float>());

    std::unique_ptr<MockFunctionStage<int, float>> innerStage1(new MockFunctionStage<int, float>());
    EXPECT_CALL(*innerStage1, Run(integers[test_set]))
        .WillOnce(testing::Return(floats[test_set]));

    innerPipeline->Complete(std::move(innerStage1));

    std::unique_ptr<MockFunctionStage<float, double>> outerStage2(new MockFunctionStage<float, double>());
    EXPECT_CALL(*outerStage2, Run(floats[test_set]))
        .WillOnce(testing::Return(doubles[test_set]));

    outerPipeline.AddStage(std::move(outerStage1))
                 .AddStage(std::move(innerPipeline))
                 .Complete(std::move(outerStage2));

    double result = outerPipeline.Run(characters[test_set]);

    ASSERT_EQ(result, doubles[test_set]);
}

/**
 * Tests putting a pipeline at the end of another pipeline
 */
TEST(SequentialPipelineTest, TestSequentialPipelineEndPipelineInPipeline) {
    INIT_SQ_CHAR_TO_DOUBLE_PIPELINE(outerPipeline);

    int test_set = 1;

    std::unique_ptr<MockFunctionStage<char, int>> outerStage1(new MockFunctionStage<char, int>());
    EXPECT_CALL(*outerStage1, Run(characters[test_set]))
        .WillOnce(testing::Return(integers[test_set]));

    std::unique_ptr<SequentialPipeline<int, double>> innerPipeline(new SequentialPipeline<int, double>());

    std::unique_ptr<MockFunctionStage<int, double>> innerStage1(new MockFunctionStage<int, double>());
    EXPECT_CALL(*innerStage1, Run(integers[test_set]))
        .WillOnce(testing::Return(doubles[test_set]));

    innerPipeline->Complete(std::move(innerStage1));

    outerPipeline.AddStage(std::move(outerStage1))
                 .Complete(std::move(innerPipeline));

    double result = outerPipeline.Run(characters[test_set]);

    ASSERT_EQ(result, doubles[test_set]);
}

TEST(SequentialPipelineTest, TestThreeSequentialPipelinesInPipeline) {
    int test_set = 1;

    SequentialPipeline<int, std::string> pipeline;

    std::unique_ptr<SequentialPipeline<int, char>> inner1(new SequentialPipeline<int, char>());

    std::unique_ptr<SequentialPipeline<int, std::string>> doubleInner(new SequentialPipeline<int, std::string>());

    std::unique_ptr<MockFunctionStage<int, std::string>> stage1(new MockFunctionStage<int, std::string>());
    EXPECT_CALL(*stage1, Run(integers[test_set]))
        .WillOnce(testing::Return(strings[test_set]));
    std::unique_ptr<MockFunctionStage<std::string, char>> stage2(new MockFunctionStage<std::string, char>());
    EXPECT_CALL(*stage2, Run(strings[test_set]))
        .WillOnce(testing::Return(characters[test_set]));

    doubleInner->Complete(std::move(stage1));

    inner1->AddStage(std::move(doubleInner)).Complete(std::move(stage2));

    std::unique_ptr<SequentialPipeline<char, double>> inner2(new SequentialPipeline<char, double>());

    std::unique_ptr<MockFunctionStage<char, double>> stage3(new MockFunctionStage<char, double>());
    EXPECT_CALL(*stage3, Run(characters[test_set]))
        .WillOnce(testing::Return(doubles[test_set]));

    inner2->Complete(std::move(stage3));

    std::unique_ptr<SequentialPipeline<double, std::string>> inner3(new SequentialPipeline<double, std::string>());

    std::unique_ptr<MockFunctionStage<double, int>> stage4(new MockFunctionStage<double, int>());
    EXPECT_CALL(*stage4, Run(doubles[test_set]))
        .WillOnce(testing::Return(integers[test_set]));
    std::unique_ptr<MockFunctionStage<int, char>> stage5(new MockFunctionStage<int, char>());
    EXPECT_CALL(*stage5, Run(integers[test_set]))
        .WillOnce(testing::Return(characters[test_set]));
    std::unique_ptr<MockFunctionStage<char, std::string>> stage6(new MockFunctionStage<char, std::string>());
    EXPECT_CALL(*stage6, Run(characters[test_set]))
        .WillOnce(testing::Return(strings[test_set]));

    inner3->AddStage(std::move(stage4)).AddStage(std::move(stage5)).Complete(std::move(stage6));

    std::string actual = pipeline.AddStage(std::move(inner1))
                                 .AddStage(std::move(inner2))
                                 .Complete(std::move(inner3))
                                 .Run(integers[test_set]);

    ASSERT_EQ(strings[test_set], actual);
}

////////////////////////////////////////////
///////// MODIFYING PIPELINE TEST //////////
////////////////////////////////////////////

/**
 * Tests running a pipeline with no stages
 */
TEST(ModifyingPipelineTest, TestModifyingPipelineRunOnEmpty) {
    INIT_M_CHAR_PIPELINE(pipeline);
    int test_input = 2;

    // no stages == fail
    ASSERT_THROW(pipeline.Run(test_input), std::runtime_error);
}

/**
 * Tests completing a pipeline, and then adding stages
 */
TEST(ModifyingPipelineTest, TestModifyingPipelineAddStageAfterComplete) {
    INIT_M_CHAR_PIPELINE(pipeline);

    std::unique_ptr<MockModifyingStage<char>> stage1(new MockModifyingStage<char>());

    std::unique_ptr<MockModifyingStage<char>> stage2(new MockModifyingStage<char>());

    pipeline.Complete(std::move(stage1));
    ASSERT_THROW(pipeline.AddStage(std::move(stage2)), std::invalid_argument);
}

/**
 * Tests completing a pipeline, and then adding stages
 */
TEST(ModifyingPipelineTest, TestModifyingPipelineCompleteAfterComplete) {
    INIT_M_CHAR_PIPELINE(pipeline);

    std::unique_ptr<MockModifyingStage<char>> stage1(new MockModifyingStage<char>());

    std::unique_ptr<MockModifyingStage<char>> stage2(new MockModifyingStage<char>());

    pipeline.Complete(std::move(stage1));
    ASSERT_THROW(pipeline.Complete(std::move(stage2)), std::invalid_argument);
}

// Invalid stages cannot be added, so skipping test

/**
 * Tests a pipeline with 1 stage
 */
TEST(ModifyingPipelineTest, TestModifyingPipelineSingleStage) {
    INIT_M_INT_PIPELINE(pipeline);

    int test_set = 0;

    int resource = integers[test_set];
    int expectedResource = integers[test_set];
    int expected = integers[test_set] + 1;

    std::unique_ptr<MockModifyingStage<int>> stage1(new MockModifyingStage<int>());
    EXPECT_CALL(*stage1, Run(resource))
            .WillOnce(testing::SetArgReferee<0>(resource + 1));

    int result = pipeline.Complete(std::move(stage1))
                         .Run(resource);

    ASSERT_EQ(expected, result);
    ASSERT_EQ(expectedResource, resource);
}

/**
 * Tests a pipeline with 3 stages
 */
TEST(ModifyingPipelineTest, TestModifyingPipelineThreeStages) {
    INIT_M_CHAR_PIPELINE(pipeline);

    int test_set = 1;

    int resource = characters[test_set];
    int expectedResource = characters[test_set];
    int expected = characters[test_set] + 7;

    std::unique_ptr<MockModifyingStage<char>> stage1(new MockModifyingStage<char>());
    EXPECT_CALL(*stage1, Run(testing::_))
        .WillOnce([](char &arg0) {
            arg0 += 1;
        });

    std::unique_ptr<MockModifyingStage<char>> stage2(new MockModifyingStage<char>());
    EXPECT_CALL(*stage2, Run(testing::_))
        .WillOnce([](char &arg0) {
            arg0 += 2;
        });

    std::unique_ptr<MockModifyingStage<char>> stage3(new MockModifyingStage<char>());
    EXPECT_CALL(*stage3, Run(testing::_))
        .WillOnce([](char &arg0) {
            arg0 += 4;
        });

    int result = pipeline.AddStage(std::move(stage1))
                         .AddStage(std::move(stage2))
                         .Complete(std::move(stage3))
                         .Run(resource);

    ASSERT_EQ(expected, result);
    ASSERT_EQ(expectedResource, resource);
}

TEST(ModifyingPipelineTest, TestModifierAtBeginningOfSequentialPipeline) {
    std::unique_ptr<ModifyingPipeline<int>> modPipeline(new ModifyingPipeline<int>());

    int test_set = 0;

    int resource = integers[test_set];
    int expected1 = integers[test_set] + 1;

    std::unique_ptr<MockModifyingStage<int>> stage1(new MockModifyingStage<int>());
    EXPECT_CALL(*stage1, Run(resource))
            .WillOnce(testing::SetArgReferee<0>(resource + 1));

    modPipeline->Complete(std::move(stage1));

    INIT_SQ_INT_TO_CHAR_PIPELINE(sqPipeline);

    std::unique_ptr<MockFunctionStage<char, double>> stage2(new MockFunctionStage<char, double>());
    EXPECT_CALL(*stage2, Run(expected1))
            .WillOnce(testing::Return(doubles[test_set]));
    std::unique_ptr<MockFunctionStage<double, char>> stage3(new MockFunctionStage<double, char>());
    EXPECT_CALL(*stage3, Run(doubles[test_set]))
            .WillOnce(testing::Return(characters[test_set]));

    int actual = sqPipeline.AddStage(std::move(modPipeline))
                           .AddStage(std::move(stage2))
                           .Complete(std::move(stage3))
                           .Run(resource);

    ASSERT_EQ(characters[test_set], actual);
}

TEST(ModifyingPipelineTest, TestModifierAtEndOfSequentialPipeline) {
    std::unique_ptr<ModifyingPipeline<char>> modPipeline(new ModifyingPipeline<char>());

    int test_set = 2;

    char resource = characters[test_set];
    char expectedResource = characters[test_set];
    char expected = characters[test_set] - 3;

    std::unique_ptr<MockModifyingStage<char>> stage1(new MockModifyingStage<char>());
    EXPECT_CALL(*stage1, Run(testing::_))
        .WillOnce([](char &arg0) {
            arg0 += 1;
        });
    std::unique_ptr<MockModifyingStage<char>> stage2(new MockModifyingStage<char>());
    EXPECT_CALL(*stage2, Run(testing::_))
        .WillOnce([](char &arg0) {
            arg0 -= 4;
        });

    modPipeline->AddStage(std::move(stage1)).Complete(std::move(stage2));

    INIT_SQ_INT_TO_CHAR_PIPELINE(sqPipeline);

    std::unique_ptr<MockFunctionStage<int, double>> stage3(new MockFunctionStage<int, double>());
    EXPECT_CALL(*stage3, Run(integers[test_set]))
            .WillOnce(testing::Return(doubles[test_set]));
    std::unique_ptr<MockFunctionStage<double, char>> stage4(new MockFunctionStage<double, char>());
    EXPECT_CALL(*stage4, Run(doubles[test_set]))
            .WillOnce(testing::Return(resource));

    int actual = sqPipeline.AddStage(std::move(stage3))
                           .AddStage(std::move(stage4))
                           .Complete(std::move(modPipeline))
                           .Run(integers[test_set]);

    ASSERT_EQ(expected, actual);
    ASSERT_EQ(expectedResource, resource);
}

TEST(ModifyingPipelineTest, TestModifierAtMiddleSequentialPipeline) {
    INIT_SQ_PIPELINE(int, double, sqPipeline);

    int test_set = 1;
    int resource = integers[test_set];
    int expectedResource = integers[test_set];
    int expectedIntermediate = ((resource * 2) % 4) + 5;

    std::unique_ptr<MockFunctionStage<int, std::string>> stage1(new MockFunctionStage<int, std::string>());
    EXPECT_CALL(*stage1, Run(integers[test_set]))
            .WillOnce(testing::Return(strings[test_set]));
    std::unique_ptr<MockFunctionStage<std::string, int>> stage2(new MockFunctionStage<std::string, int>());
    EXPECT_CALL(*stage2, Run(strings[test_set]))
            .WillOnce(testing::Return(resource));

    std::unique_ptr<ModifyingPipeline<int>> modPipeline(new ModifyingPipeline<int>());

    std::unique_ptr<MockModifyingStage<int>> stage3(new MockModifyingStage<int>());
    EXPECT_CALL(*stage3, Run(testing::_))
        .WillOnce([](int &arg0) {
            arg0 *= 2;
        });

    std::unique_ptr<MockModifyingStage<int>> stage4(new MockModifyingStage<int>());
    EXPECT_CALL(*stage4, Run(testing::_))
        .WillOnce([](int &arg0) {
            arg0 %= 4;
        });

    std::unique_ptr<MockModifyingStage<int>> stage5(new MockModifyingStage<int>());
    EXPECT_CALL(*stage5, Run(testing::_))
        .WillOnce([](int &arg0) {
            arg0 += 5;
        });

    modPipeline->AddStage(std::move(stage3)).AddStage(std::move(stage4)).Complete(std::move(stage5));

    std::unique_ptr<MockFunctionStage<int, double>> stage6(new MockFunctionStage<int, double>());
    EXPECT_CALL(*stage6, Run(expectedIntermediate))
            .WillOnce(testing::Return(doubles[test_set]));

    double actual = sqPipeline.AddStage(std::move(stage1))
                              .AddStage(std::move(stage2))
                              .AddStage(std::move(modPipeline))
                              .Complete(std::move(stage6))
                              .Run(integers[test_set]);

    ASSERT_EQ(doubles[test_set], actual);
    ASSERT_EQ(expectedResource, resource);
}

////////////////////////////////////////////
////////// NOMINAL PIPELINE TEST ///////////
////////////////////////////////////////////

TEST(NominalPipelineTest, TestPipelinesAsStages) {
    // skip calibration stage, because it only has 1 stage

    DistancePipeline distancePipeline;
    PositionVector expectedVec(1, 2, 3);

    // Here, we make a single stage that looks like the distance pipeline
    std::unique_ptr<MockFunctionStage<Image, PositionVector>>
        distanceStage(new MockFunctionStage<Image, PositionVector>());
    EXPECT_CALL(*distanceStage, Run(testing::_))
        .WillOnce(testing::Return(expectedVec));

    PositionVector actualVec(distancePipeline.Complete(std::move(distanceStage))
                                             .Run({}));

    ASSERT_VEC3_EQ_DEFAULT(expectedVec, actualVec);

    std::unique_ptr<OrbitPipeline> orbitPipeline(new OrbitPipeline());
    LocationRecords expectedLR;

    // Again, we make a single stage regardless of how many there
    // actually are in the pipeline
    std::unique_ptr<MockFunctionStage<LocationRecords, LocationRecords>>
        orbitStage(new MockFunctionStage<LocationRecords, LocationRecords>());
    EXPECT_CALL(*orbitStage, Run(testing::_))
        .WillOnce(testing::Return(expectedLR));

    orbitPipeline->Complete(std::move(orbitStage));

    OrbitPipeline wrapperOrbitPipeline;
    wrapperOrbitPipeline.Complete(std::move(orbitPipeline));
    LocationRecords actualLR(wrapperOrbitPipeline.Run({}));

    ASSERT_THAT(expectedLR, LocationRecordsEqual(actualLR));
}

TEST(NominalPipelineTest, TestNominalPipelinesWrapped) {
    std::unique_ptr<CalibrationPipeline> calibrationPipeline(new CalibrationPipeline());
    Quaternion expectedQuat{1, 2, 3, 4};

    std::unique_ptr<MockFunctionStage<Orientations, Quaternion>>
        calibrationStage(new MockFunctionStage<Orientations, Quaternion>());
    EXPECT_CALL(*calibrationStage, Run(testing::_))
        .WillOnce(testing::Return(expectedQuat));

    calibrationPipeline->Complete(std::move(calibrationStage));

    CalibrationPipeline wrapperCalibrationPipeline;
    wrapperCalibrationPipeline.Complete(std::move(calibrationPipeline));
    Quaternion actualQuat(wrapperCalibrationPipeline.Run({}));

    ASSERT_QUAT_EQ_DEFAULT(expectedQuat, actualQuat);

    std::unique_ptr<DistancePipeline> distancePipeline(new DistancePipeline());
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

    distancePipeline->AddStage(std::move(edgeDetectionStage))
                    .AddStage(std::move(distanceStage))
                    .Complete(std::move(vectorStage));

    DistancePipeline wrapperDistancePipeline;
    wrapperDistancePipeline.Complete(std::move(distancePipeline));
    PositionVector actualVec(wrapperDistancePipeline.Run({}));

    ASSERT_VEC3_EQ_DEFAULT(expectedVec, actualVec);

    std::unique_ptr<OrbitPipeline> orbitPipeline(new OrbitPipeline());
    LocationRecords expectedLR;

    std::unique_ptr<MockFunctionStage<LocationRecords, LocationRecords>>
        undefinedOrbitStage(new MockFunctionStage<LocationRecords, LocationRecords>());
    EXPECT_CALL(*undefinedOrbitStage, Run(testing::_))
        .WillOnce(testing::Return(expectedLR));

    orbitPipeline->Complete(std::move(undefinedOrbitStage));

    OrbitPipeline wrapperOrbitPipeline;
    wrapperOrbitPipeline.Complete(std::move(orbitPipeline));
    LocationRecords actualLR(wrapperOrbitPipeline.Run({}));

    ASSERT_THAT(expectedLR, LocationRecordsEqual(actualLR));
}

TEST(NominalPipelineTest, TestNominalPipelinesModifyingPipelinesAsStages) {
    // First, get all modifying stages as normal function stages
    std::unique_ptr<MockFunctionStage<Points, Points>> edgeFilteringStage(
        new MockFunctionStage<Points, Points>());
    EXPECT_CALL(*edgeFilteringStage, Run(testing::_))
        .WillOnce(testing::Return(points));

    // Second, put them into a pipeline
    INIT_SQ_PIPELINE(Points, Points, edgeFilteringPipeline);
    edgeFilteringPipeline.Complete(std::move(edgeFilteringStage));

    // Third, call and verify the output
    Points actual(edgeFilteringPipeline.Run(points));

    // What's happening here is that we use are getting all our elements
    // in our expected output, wrapping them in a Matcher, and then
    // asserting that the resulting array has the same elements in any order
    std::vector<testing::Matcher<Vec2>> matchers;
    std::transform(points.begin(), points.end(), std::back_inserter(matchers),
                [](const Vec2& val) {
                    return Vec2Equal(val);
                });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

}  // namespace found
