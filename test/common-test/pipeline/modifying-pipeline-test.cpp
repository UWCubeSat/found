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

    pipeline.Complete(*stage1);
    ASSERT_THROW(pipeline.AddStage(*stage2), std::invalid_argument);
}

/**
 * Tests completing a pipeline, and then adding stages
 */
TEST(ModifyingPipelineTest, TestModifyingPipelineCompleteAfterComplete) {
    INIT_M_CHAR_PIPELINE(pipeline);

    std::unique_ptr<MockModifyingStage<char>> stage1(new MockModifyingStage<char>());

    std::unique_ptr<MockModifyingStage<char>> stage2(new MockModifyingStage<char>());

    pipeline.Complete(*stage1);
    ASSERT_THROW(pipeline.Complete(*stage2), std::invalid_argument);
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

    int result = pipeline.Complete(*stage1)
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

    int result = pipeline.AddStage(*stage1)
                         .AddStage(*stage2)
                         .Complete(*stage3)
                         .Run(resource);

    ASSERT_EQ(expected, result);
    ASSERT_EQ(expectedResource, resource);
}

TEST(ModifyingPipelineTest, TestModifierAtBeginningOfSequentialPipeline) {
    INIT_M_INT_PIPELINE(modPipeline);

    int test_set = 0;

    int resource = integers[test_set];
    int expected1 = integers[test_set] + 1;

    std::unique_ptr<MockModifyingStage<int>> stage1(new MockModifyingStage<int>());
    EXPECT_CALL(*stage1, Run(resource))
            .WillOnce(testing::SetArgReferee<0>(resource + 1));

    modPipeline.Complete(*stage1);

    INIT_SQ_INT_TO_CHAR_PIPELINE(sqPipeline);

    std::unique_ptr<MockFunctionStage<char, double>> stage2(new MockFunctionStage<char, double>());
    EXPECT_CALL(*stage2, Run(expected1))
            .WillOnce(testing::Return(doubles[test_set]));
    std::unique_ptr<MockFunctionStage<double, char>> stage3(new MockFunctionStage<double, char>());
    EXPECT_CALL(*stage3, Run(doubles[test_set]))
            .WillOnce(testing::Return(characters[test_set]));

    int actual = sqPipeline.AddStage(modPipeline)
                           .AddStage(*stage2)
                           .Complete(*stage3)
                           .Run(resource);

    ASSERT_EQ(characters[test_set], actual);
}

TEST(ModifyingPipelineTest, TestModifierAtEndOfSequentialPipeline) {
    INIT_M_CHAR_PIPELINE(modPipeline);

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

    modPipeline.AddStage(*stage1).Complete(*stage2);

    INIT_SQ_INT_TO_CHAR_PIPELINE(sqPipeline);

    std::unique_ptr<MockFunctionStage<int, double>> stage3(new MockFunctionStage<int, double>());
    EXPECT_CALL(*stage3, Run(integers[test_set]))
            .WillOnce(testing::Return(doubles[test_set]));
    std::unique_ptr<MockFunctionStage<double, char>> stage4(new MockFunctionStage<double, char>());
    EXPECT_CALL(*stage4, Run(doubles[test_set]))
            .WillOnce(testing::Return(resource));

    int actual = sqPipeline.AddStage(*stage3)
                           .AddStage(*stage4)
                           .Complete(modPipeline)
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

    INIT_M_INT_PIPELINE(modPipeline);

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

    modPipeline.AddStage(*stage3).AddStage(*stage4).Complete(*stage5);

    std::unique_ptr<MockFunctionStage<int, double>> stage6(new MockFunctionStage<int, double>());
    EXPECT_CALL(*stage6, Run(expectedIntermediate))
            .WillOnce(testing::Return(doubles[test_set]));

    double actual = sqPipeline.AddStage(*stage1)
                              .AddStage(*stage2)
                              .AddStage(modPipeline)
                              .Complete(*stage6)
                              .Run(integers[test_set]);

    ASSERT_EQ(doubles[test_set], actual);
    ASSERT_EQ(expectedResource, resource);
}

TEST(ModifyingPipelineTest, TestSequentialAndModifierPipelinesinSequentialPipeline) {
    INIT_SQ_PIPELINE(char, double, sqPipeline);

    int test_set = 0;
    int resource = integers[test_set];
    int expectedResource = integers[test_set];
    int expectedIntermediate = ((resource * 2) % 4) + 5;

    INIT_SQ_PIPELINE(char, int, innerPipeline);

    std::unique_ptr<MockFunctionStage<char, std::string>> stage1(new MockFunctionStage<char, std::string>());
    EXPECT_CALL(*stage1, Run(characters[test_set]))
            .WillOnce(testing::Return(strings[test_set]));
    std::unique_ptr<MockFunctionStage<std::string, int>> stage2(new MockFunctionStage<std::string, int>());
    EXPECT_CALL(*stage2, Run(strings[test_set]))
            .WillOnce(testing::Return(resource));

    innerPipeline.AddStage(*stage1).Complete(*stage2);

    INIT_M_INT_PIPELINE(modPipeline);

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

    modPipeline.AddStage(*stage3).AddStage(*stage4).Complete(*stage5);

    std::unique_ptr<MockFunctionStage<int, double>> stage6(new MockFunctionStage<int, double>());
    EXPECT_CALL(*stage6, Run(expectedIntermediate))
            .WillOnce(testing::Return(doubles[test_set]));

    double actual = sqPipeline.AddStage(innerPipeline)
                              .AddStage(modPipeline)
                              .Complete(*stage6)
                              .Run(characters[test_set]);

    ASSERT_EQ(doubles[test_set], actual);
    ASSERT_EQ(expectedResource, resource);
}

}  // namespace found
