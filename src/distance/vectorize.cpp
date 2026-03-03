#include "distance/vectorize.hpp"

#include "common/logging.hpp"

namespace found {

PositionVector LOSTVectorGenerationAlgorithm::Run(const PositionVector &x_E) {
    // orientation is stored as a forwards (camera → celestial) rotation, so apply it
    // directly to map x_E from the camera frame into the celestial frame.
    return -(this->orientation * x_E);
}

}  // namespace found
