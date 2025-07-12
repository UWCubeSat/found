#ifndef TEST_COMMON_MOCKS_DISTANCE_MOCKS_HPP_
#define TEST_COMMON_MOCKS_DISTANCE_MOCKS_HPP_

#include "common/style.hpp"

#include "distance/edge.hpp"
#include "distance/distance.hpp"
#include "distance/vectorize.hpp"

namespace found {

class MockEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
 public:
    MOCK_METHOD(Points, Run, (const Image& image), (override));
};

class MockDistanceDeterminationAlgorithm : public DistanceDeterminationAlgorithm {
 public:
    MOCK_METHOD(PositionVector, Run, (const Points& points), (override));
};

class MockVectorGenerationAlgorithm : public VectorGenerationAlgorithm {
 public:
    MOCK_METHOD(PositionVector, Run, (const PositionVector& points), (override));
};

}  // namespace found

#endif  // TEST_COMMON_MOCKS_DISTANCE_MOCKS_HPP_
