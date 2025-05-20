#include <gmock/gmock.h>

#include "src/common/pipeline.hpp"

namespace found {

template <typename Input, typename Output>
class MockStage : public Stage<Input, Output> {
 public:
    MOCK_METHOD(Output, Run, (const Input &), (override));
};

}  // namespace found
