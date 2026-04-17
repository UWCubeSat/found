#ifndef TEST_COMMON_MOCKS_PIPELINE_MOCKS_HPP_
#define TEST_COMMON_MOCKS_PIPELINE_MOCKS_HPP_

#include <gmock/gmock.h>

#include <utility>
#include <memory>

#include "src/common/pipeline/stages.hpp"

namespace found {

// Casts a unique_ptr<Stage> to unique_ptr<FunctionStage<I, O>>
template<typename I, typename O, typename Stage>
std::unique_ptr<FunctionStage<I, O>> ToFunctionStage(std::unique_ptr<Stage> stage) {
    return std::unique_ptr<FunctionStage<I, O>>(std::move(stage));
}

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

template <typename T>
class MockExposedModifyingPipeline : public ModifyingPipeline<T> {
 public:
    void SetExternalProduct(T *productPtr) { this->product = productPtr; }
    bool HasFinalProduct() const { return this->finalProduct.has_value(); }
};

}  // namespace found

#endif  // TEST_COMMON_MOCKS_PIPELINE_MOCKS_HPP_
