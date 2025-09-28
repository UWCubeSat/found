#include "distance/vectorize.hpp"

#include "common/logging.hpp"

namespace found {

PositionVector LOSTVectorGenerationAlgorithm::Run(const PositionVector &x_E) {
    // Use the conjugate here, since the orientation is a backwards rotation. In
    // other words, the orientation is a rotation from the celestial frame to the
    // camera frame, but we want to go the other way.
    return -this->orientation.Rotate(x_E);
}

}  // namespace found
