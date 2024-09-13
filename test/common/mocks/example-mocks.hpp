#include <gmock/gmock.h>

#include "src/distance/edge.hpp"

namespace found {

class MockEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
 public:
    virtual ~MockEdgeDetectionAlgorithm() {}
    MOCK_METHOD(Points, Run, (unsigned char* image), (override));
};

}  // namespace found
