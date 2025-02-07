#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <vector>
#include <memory>

#include "src/pipeline/pipeline.hpp"

#include "test/common/constants/pipeline-constants.hpp"
#include "test/common/mocks/pipeline-mocks.hpp"

namespace found {

class PipelineTest : public testing::Test {
 protected:
    // System dependencies

    // Our vector of pipeline stages
    std::vector<std::reference_wrapper<Action>> stages;

    // System under test: Pipeline

    // Pipeline<Input, Output> pipeline;

    void SetUp() override {
        stages = std::vector<std::reference_wrapper<Action>>();
        // pipeline = Pipeline<Input, Output>(stages);
    }
};

/**
 * Tets to running a pipeline with no stages
 */
TEST_F(PipelineTest, TestPipelineRunOnEmpty) {
    INIT_INT_TO_CHAR_PIPELINE(pipeline, stages);
    int test_input = 2;

    // no stages == fail
    ASSERT_THROW(pipeline.Run(test_input), std::runtime_error);
}

/**
 * Tests completing a pipeline, and then adding stages
 */
TEST_F(PipelineTest, TestPipelineAddStageAfterComplete) {
    INIT_CHAR_TO_DOUBLE_PIPELINE(pipeline, stages);

    std::unique_ptr<MockStage<char, double>> stage1(new MockStage<char, double>());

    std::unique_ptr<MockStage<int, double>> stage2(new MockStage<int, double>());

    pipeline.Complete(*stage1);
    ASSERT_THROW(pipeline.AddStage(*stage2), std::invalid_argument);
}

/**
 * Tests adding an invalid first stage
 */
TEST_F(PipelineTest, TestPipelineInvalidFirstStage) {
    INIT_CHAR_TO_DOUBLE_PIPELINE(pipeline, stages);

    std::unique_ptr<MockStage<double, int>> stage1(new MockStage<double, int>());

    ASSERT_THROW(pipeline.AddStage(*stage1), std::invalid_argument);
}

/**
 * Tests a pipeline with 1 stage
 */
TEST_F(PipelineTest, TestPipelineSingleStage) {
    INIT_INT_TO_CHAR_PIPELINE(pipeline, stages);

    int test_set = 1;

    std::unique_ptr<MockStage<int, char>> stage1(new MockStage<int, char>());
    EXPECT_CALL(*stage1, Run(integers[test_set]))
        .WillOnce(testing::Return(characters[test_set]));

    char result = pipeline.Complete(*stage1)
                          .Run(integers[test_set]);

    ASSERT_EQ(characters[test_set], result);
}

/**
 * Tests a pipeline with 2 stages
 */
TEST_F(PipelineTest, TestPipelineTwoStage) {
    // Make our pipeline and inject our object
    INIT_INT_TO_CHAR_PIPELINE(pipeline, stages);

    // Define our selected test parameters (from our constants
    // file)
    int test_set = 0;

    // Make our mocks and register it so that
    // it returns what we want. Our mocks are Stage
    // objects, and we've created a Mock Stage (MockStage)
    // that you can use to mock a stage (so you don't have
    // to write one out). See test/common/mocks/pipeline-mocks.hpp
    // for more information

    // The first stage goes from an integer to a string
    std::unique_ptr<MockStage<int, std::string>> stage1(new MockStage<int, std::string>());
    EXPECT_CALL(*stage1, Run(integers[test_set]))
        .WillOnce(testing::Return(strings[test_set]));
    // This line (.WillOnce(...);), we have here to make sure that this function is only called once

    // The second stage takes a string and returns
    // a char
    std::unique_ptr<MockStage<std::string, char>> stage2(new MockStage<std::string, char>());
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
TEST_F(PipelineTest, TestPipelineTwoStageOther) {
    INIT_DOUBLE_TO_STRING_PIPELINE(pipeline, stages);

    int test_set = 1;

    std::unique_ptr<MockStage<double, float>> stage1(new MockStage<double, float>());
    EXPECT_CALL(*stage1, Run(doubles[test_set]))
        .WillOnce(testing::Return(floats[test_set]));

    std::unique_ptr<MockStage<float, std::string>> stage2(new MockStage<float, std::string>());
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
TEST_F(PipelineTest, TestPipelineThreeStage) {
    INIT_CHAR_TO_DOUBLE_PIPELINE(pipeline, stages);

    int test_set = 2;

    std::unique_ptr<MockStage<char, int>> stage1(new MockStage<char, int>());
    EXPECT_CALL(*stage1, Run(characters[test_set]))
        .WillOnce(testing::Return(integers[test_set]));

    std::unique_ptr<MockStage<int, std::string>> stage2(new MockStage<int, std::string>());
    EXPECT_CALL(*stage2, Run(integers[test_set]))
        .WillOnce(testing::Return(strings[test_set]));

    std::unique_ptr<MockStage<std::string, double>> stage3(new MockStage<std::string, double>());
    EXPECT_CALL(*stage3, Run(strings[test_set]))
        .WillOnce(testing::Return(doubles[test_set]));

    double result = pipeline.AddStage(*stage1)
                          .AddStage(*stage2)
                          .Complete(*stage3)
                          .Run(characters[test_set]);
    ASSERT_EQ(doubles[test_set], result);
}

/**
 * Tests putting a pipline in the beginning of another pipeline
 */
TEST_F(PipelineTest, TestPipelineBeginningPipelineInPipeline) {
    INIT_CHAR_TO_DOUBLE_PIPELINE(outerPipeline, stages);

    int test_set = 0;

    // begin pipeline
    std::vector<std::reference_wrapper<Action>> innerStages;
    INIT_PIPELINE(char, int, innerPipeline, innerStages);

    std::unique_ptr<MockStage<char, int>> innerStage1(new MockStage<char, int>());
    EXPECT_CALL(*innerStage1, Run(characters[test_set]))
        .WillOnce(testing::Return(integers[test_set]));

    innerPipeline.Complete(*innerStage1);

    std::unique_ptr<MockStage<int, double>> outerStage1(new MockStage<int, double>());
    EXPECT_CALL(*outerStage1, Run(integers[test_set]))
        .WillOnce(testing::Return(doubles[test_set]));

    outerPipeline.AddStage(innerPipeline)
                 .Complete(*outerStage1);

    double result = outerPipeline.Run(characters[test_set]);

    ASSERT_EQ(result, doubles[test_set]);
}

/**
 * Tests adding a Pipeline into the middle of another Pipeline
 */
TEST_F(PipelineTest, TestPipelineMiddlePipelineInPipeline) {
    INIT_CHAR_TO_DOUBLE_PIPELINE(outerPipeline, stages);

    int test_set = 2;

    std::unique_ptr<MockStage<char, int>> outerStage1(new MockStage<char, int>());
    EXPECT_CALL(*outerStage1, Run(characters[test_set]))
        .WillOnce(testing::Return(integers[test_set]));

    // middle pipeline
    std::vector<std::reference_wrapper<Action>> innerStages;
    INIT_PIPELINE(int, float, innerPipeline, innerStages);

    std::unique_ptr<MockStage<int, float>> innerStage1(new MockStage<int, float>());
    EXPECT_CALL(*innerStage1, Run(integers[test_set]))
        .WillOnce(testing::Return(floats[test_set]));

    innerPipeline.Complete(*innerStage1);

    std::unique_ptr<MockStage<float, double>> outerStage2(new MockStage<float, double>());
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
TEST_F(PipelineTest, TestPipelineEndPipelineInPipeline) {
    INIT_CHAR_TO_DOUBLE_PIPELINE(outerPipeline, stages);

    int test_set = 1;

    std::unique_ptr<MockStage<char, int>> outerStage1(new MockStage<char, int>());
    EXPECT_CALL(*outerStage1, Run(characters[test_set]))
        .WillOnce(testing::Return(integers[test_set]));

    // ending pipeline
    std::vector<std::reference_wrapper<Action>> innerStages;
    INIT_PIPELINE(int, double, innerPipeline, innerStages);

    std::unique_ptr<MockStage<int, double>> innerStage1(new MockStage<int, double>());
    EXPECT_CALL(*innerStage1, Run(integers[test_set]))
        .WillOnce(testing::Return(doubles[test_set]));

    innerPipeline.Complete(*innerStage1);

    outerPipeline.AddStage(*outerStage1)
                 .Complete(innerPipeline);

    double result = outerPipeline.Run(characters[test_set]);

    ASSERT_EQ(result, doubles[test_set]);
}

}  // namespace found
