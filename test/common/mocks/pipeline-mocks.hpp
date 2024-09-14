#include <gmock/gmock.h>

#include "src/pipeline/pipeline.hpp"

namespace found {

template <typename Input, typename Output>
class MockStage : public Stage<Input, Output> {
 public:
    MOCK_METHOD(Output, Run, (Input), (override));
};

}  // namespace found
