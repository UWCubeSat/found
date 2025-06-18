#include "distance/vectorize.hpp"

#include "common/logging.hpp"

namespace found {

PositionVector LOSTVectorGenerationAlgorithm::Run(const PositionVector &x_E) {
    // Use the conjugate here, since we want it in terms of the axis
    // given by the quaternion
    return -this->orientation.Rotate(x_E);
}

}  // namespace found
