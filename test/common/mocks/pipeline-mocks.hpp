#ifndef TEST_COMMON_MOCKS_PIPELINE_MOCKS_HPP_
#define TEST_COMMON_MOCKS_PIPELINE_MOCKS_HPP_

#include <gmock/gmock.h>

#include "src/common/pipeline/stages.hpp"

namespace found {

template <typename Input, typename Output>
class MockFunctionStage : public FunctionStage<Input, Output> {
 public:
    MOCK_METHOD(Output, Run, (const Input &), (override));
};

template <typename T>
class MockModifyingStage : public ModifyingStage<T> {
 public:
    MOCK_METHOD(void, Run, (T &), (override));
};

}  // namespace found

#endif  // TEST_COMMON_MOCKS_PIPELINE_MOCKS_HPP_
