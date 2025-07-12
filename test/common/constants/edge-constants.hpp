#ifndef TEST_COMMON_CONSTANTS_EDGE_CONSTANTS_HPP_
#define TEST_COMMON_CONSTANTS_EDGE_CONSTANTS_HPP_

#include "src/common/decimal.hpp"

#include "src/distance/edge.hpp"

namespace found {

const decimal offset = 0.5;

SimpleEdgeDetectionAlgorithm minimalSEDA(5, 1, 0);
SimpleEdgeDetectionAlgorithm thickerBorderSEDA(5, 2, 0);
SimpleEdgeDetectionAlgorithm offsetSEDA(5, 1, offset);

}

#endif  // TEST_COMMON_CONSTANTS_EDGE_CONSTANTS_HPP_
