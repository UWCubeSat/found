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

TEST_F(PipelineTest, SimplePipeline) {
    // Make our pipeline and inject our object
    Pipeline<int, char> pipeline(stages);

    // Make our mocks and register it so that
    // it returns what we want. Our mocks are Stage
    // objects, and we've created a Mock Stage (MockStage)
    // that you can use to mock a stage (so you don't have
    // to write one out). See test/common/mocks/pipeline-mocks.hpp
    // for more information

    // The first stage goes from an integer to a string
    std::unique_ptr<MockStage<int, std::string>> stage1(new MockStage<int, std::string>());
    EXPECT_CALL(*stage1, Run(integers[0]))
        .WillOnce(testing::Return(strings[0]));
    // This line (.WillOnce(...);), we have here to make sure that this function is only called once

    // The second stage takes a string and returns
    // a char
    std::unique_ptr<MockStage<std::string, char>> stage2(new MockStage<std::string, char>());
    EXPECT_CALL(*stage2, Run(strings[0]))
        .WillOnce(testing::Return(characters[0]));

    // Now, we construct the pipeline and run it
    char result = pipeline.AddStage(*stage1)
                          .Complete(*stage2)
                          .Run(integers[0]);
    // And we verify the result
    ASSERT_EQ(characters[0], result);
}

}  // namespace found