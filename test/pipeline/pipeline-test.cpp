#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <vector>

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

TEST_F(PipelineTest, IntToStringToCharPipeline) {
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
// DOUBLE > FLOAT > STRING
TEST_F(PipelineTest, DoubleToFloatToStringPipeline) {
    INIT_DOUBLE_TO_STRING_PIPELINE(pipeline, stages);

    int test_set = 0;

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

// CHAR > INT > Double
TEST_F(PipelineTest, CharToIntToDoublePipeline) {
    INIT_CHAR_TO_DOUBLE_PIPELINE(pipeline, stages);

    int test_set = 0;

    std::unique_ptr<MockStage<char, int>> stage1(new MockStage<char, int>());
    EXPECT_CALL(*stage1, Run(characters[test_set]))
        .WillOnce(testing::Return(integers[test_set]));

    std::unique_ptr<MockStage<int, double>> stage2(new MockStage<int, double>());
    EXPECT_CALL(*stage2, Run(integers[test_set]))
        .WillOnce(testing::Return(doubles[test_set]));

    double result = pipeline.AddStage(*stage1)
                          .Complete(*stage2)
                          .Run(characters[test_set]);
    ASSERT_EQ(doubles[test_set], result);
}

// one stage pipeline
TEST_F(PipelineTest, SingleStagePipeline) {
    INIT_INT_TO_CHAR_PIPELINE(pipeline, stages);

    int test_set = 0;

    std::unique_ptr<MockStage<int, char>> stage1(new MockStage<int, char>());
    EXPECT_CALL(*stage1, Run(integers[test_set]))
        .WillOnce(testing::Return(characters[test_set]));

    char result = pipeline.Complete(*stage1)
                          .Run(integers[test_set]);

    ASSERT_EQ(characters[test_set], result);
}

// empty pipeline > expecting failure
TEST_F(PipelineTest, EmptyPipelineThrowsError) {
    INIT_INT_TO_CHAR_PIPELINE(pipeline, stages);
    int test_input = 0;

    // no stages == fail
    ASSERT_THROW(pipeline.Run(test_input), std::runtime_error);
}

TEST_F(PipelineTest, CompleteBeforeAddStagePipeline) {
    INIT_CHAR_TO_DOUBLE_PIPELINE(pipeline, stages);

    std::unique_ptr<MockStage<char, double>> stage1(new MockStage<char, double>());

    std::unique_ptr<MockStage<int, double>> stage2(new MockStage<int, double>());

    pipeline.Complete(*stage1);
    ASSERT_THROW(pipeline.AddStage(*stage2), std::invalid_argument);
}

TEST_F(PipelineTest, InvalidFirstStageInputType) {
    INIT_CHAR_TO_DOUBLE_PIPELINE(pipeline, stages);

    std::unique_ptr<MockStage<double, int>> stage1(new MockStage<double, int>());

    ASSERT_THROW(pipeline.AddStage(*stage1), std::invalid_argument);
}

// beginning inner pipeline
TEST_F(PipelineTest, BeginningPipelineInPipeline) {
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

// end inner pipeline
TEST_F(PipelineTest, EndPipelineInPipeline) {
    INIT_CHAR_TO_DOUBLE_PIPELINE(outerPipeline, stages);

    int test_set = 0;

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

TEST_F(PipelineTest, MiddlePipelineInPipeline) {
    INIT_CHAR_TO_DOUBLE_PIPELINE(outerPipeline, stages);

    int test_set = 0;

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

}  // namespace found
