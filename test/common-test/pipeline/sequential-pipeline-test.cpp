#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <vector>
#include <memory>

#include "src/common/pipeline/pipelines.hpp"

#include "test/common/constants/pipeline-constants.hpp"
#include "test/common/mocks/pipeline-mocks.hpp"

namespace found {

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

    pipeline.Complete(*stage1);
    ASSERT_THROW(pipeline.AddStage(*stage2), std::invalid_argument);
}

/**
 * Tests compelteing a pipeline after completing it
 */
TEST(SequentialPipelineTest, TestSequentialPipelineCompleteAfterComplete) {
    INIT_SQ_CHAR_TO_DOUBLE_PIPELINE(pipeline);

    std::unique_ptr<MockFunctionStage<char, double>> stage1(new MockFunctionStage<char, double>());

    std::unique_ptr<MockFunctionStage<int, double>> stage2(new MockFunctionStage<int, double>());

    pipeline.Complete(*stage1);
    ASSERT_THROW(pipeline.Complete(*stage2), std::invalid_argument);
}

/**
 * Tests adding an invalid first stage
 */
TEST(SequentialPipelineTest, TestSequentialPipelineInvalidFirstStage) {
    INIT_SQ_CHAR_TO_DOUBLE_PIPELINE(pipeline);

    std::unique_ptr<MockFunctionStage<double, int>> stage1(new MockFunctionStage<double, int>());

    ASSERT_THROW(pipeline.AddStage(*stage1), std::invalid_argument);
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

    char result = pipeline.Complete(*stage1)
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
    char result = pipeline.AddStage(*stage1)
                          .Complete(*stage2)
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

    std::string result = pipeline.AddStage(*stage1)
                          .Complete(*stage2)
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

    double result = pipeline.AddStage(*stage1)
                          .AddStage(*stage2)
                          .Complete(*stage3)
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

    std::string result = pipeline.AddStage(*stage1)
                                 .AddStage(*stage2)
                                 .AddStage(*stage3)
                                 .Complete(*stage4)
                                 .Run(doubles[test_set]);
    ASSERT_EQ(strings[test_set], result);
}

/**
 * Tests putting a pipline in the beginning of another pipeline
 */
TEST(SequentialPipelineTest, TestSequentialPipelineBeginningPipelineInPipeline) {
    INIT_SQ_CHAR_TO_DOUBLE_PIPELINE(outerPipeline);

    int test_set = 0;

    // begin pipeline
    INIT_SQ_PIPELINE(char, int, innerPipeline);

    std::unique_ptr<MockFunctionStage<char, char>> innerStage1(new MockFunctionStage<char, char>());
    EXPECT_CALL(*innerStage1, Run(characters[test_set]))
        .WillOnce(testing::Return(characters[test_set]));

    std::unique_ptr<MockFunctionStage<char, int>> innerStage2(new MockFunctionStage<char, int>());
    EXPECT_CALL(*innerStage2, Run(characters[test_set]))
        .WillOnce(testing::Return(integers[test_set]));

    innerPipeline.AddStage(*innerStage1).Complete(*innerStage2);

    std::unique_ptr<MockFunctionStage<int, int>> outerStage1(new MockFunctionStage<int, int>());
    EXPECT_CALL(*outerStage1, Run(integers[test_set]))
        .WillOnce(testing::Return(integers[test_set]));

    std::unique_ptr<MockFunctionStage<int, double>> outerStage2(new MockFunctionStage<int, double>());
    EXPECT_CALL(*outerStage2, Run(integers[test_set]))
        .WillOnce(testing::Return(doubles[test_set]));

    outerPipeline.AddStage(innerPipeline)
                 .AddStage(*outerStage1)
                 .Complete(*outerStage2);

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

    // middle pipeline
    INIT_SQ_PIPELINE(int, float, innerPipeline);

    std::unique_ptr<MockFunctionStage<int, float>> innerStage1(new MockFunctionStage<int, float>());
    EXPECT_CALL(*innerStage1, Run(integers[test_set]))
        .WillOnce(testing::Return(floats[test_set]));

    innerPipeline.Complete(*innerStage1);

    std::unique_ptr<MockFunctionStage<float, double>> outerStage2(new MockFunctionStage<float, double>());
    EXPECT_CALL(*outerStage2, Run(floats[test_set]))
        .WillOnce(testing::Return(doubles[test_set]));

    outerPipeline.AddStage(*outerStage1)
                 .AddStage(innerPipeline)
                 .Complete(*outerStage2);

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

    // ending pipeline
    INIT_SQ_PIPELINE(int, double, innerPipeline);

    std::unique_ptr<MockFunctionStage<int, double>> innerStage1(new MockFunctionStage<int, double>());
    EXPECT_CALL(*innerStage1, Run(integers[test_set]))
        .WillOnce(testing::Return(doubles[test_set]));

    innerPipeline.Complete(*innerStage1);

    outerPipeline.AddStage(*outerStage1)
                 .Complete(innerPipeline);

    double result = outerPipeline.Run(characters[test_set]);

    ASSERT_EQ(result, doubles[test_set]);
}

TEST(SequentialPipelineTest, TestThreeSequentialPipelinesInPipeline) {
    int test_set = 1;

    INIT_SQ_PIPELINE(int, std::string, pipeline);

    INIT_SQ_INT_TO_CHAR_PIPELINE(inner1);

    INIT_SQ_PIPELINE(int, std::string, doubleInner);

    std::unique_ptr<MockFunctionStage<int, std::string>> stage1(new MockFunctionStage<int, std::string>());
    EXPECT_CALL(*stage1, Run(integers[test_set]))
        .WillOnce(testing::Return(strings[test_set]));
    std::unique_ptr<MockFunctionStage<std::string, char>> stage2(new MockFunctionStage<std::string, char>());
    EXPECT_CALL(*stage2, Run(strings[test_set]))
        .WillOnce(testing::Return(characters[test_set]));

    doubleInner.Complete(*stage1);

    inner1.AddStage(doubleInner).Complete(*stage2);

    INIT_SQ_CHAR_TO_DOUBLE_PIPELINE(inner2);

    std::unique_ptr<MockFunctionStage<char, double>> stage3(new MockFunctionStage<char, double>());
    EXPECT_CALL(*stage3, Run(characters[test_set]))
        .WillOnce(testing::Return(doubles[test_set]));

    inner2.Complete(*stage3);

    INIT_SQ_DOUBLE_TO_STRING_PIPELINE(inner3);

    std::unique_ptr<MockFunctionStage<double, int>> stage4(new MockFunctionStage<double, int>());
    EXPECT_CALL(*stage4, Run(doubles[test_set]))
        .WillOnce(testing::Return(integers[test_set]));
    std::unique_ptr<MockFunctionStage<int, char>> stage5(new MockFunctionStage<int, char>());
    EXPECT_CALL(*stage5, Run(integers[test_set]))
        .WillOnce(testing::Return(characters[test_set]));
    std::unique_ptr<MockFunctionStage<char, std::string>> stage6(new MockFunctionStage<char, std::string>());
    EXPECT_CALL(*stage6, Run(characters[test_set]))
        .WillOnce(testing::Return(strings[test_set]));

    inner3.AddStage(*stage4).AddStage(*stage5).Complete(*stage6);

    std::string actual = pipeline.AddStage(inner1).AddStage(inner2).Complete(inner3).Run(integers[test_set]);

    ASSERT_EQ(strings[test_set], actual);
}

}  // namespace found
