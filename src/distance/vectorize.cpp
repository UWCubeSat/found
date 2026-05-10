#include "distance/vectorize.hpp"

#include "common/logging.hpp"

namespace found {

PositionVector LOSTVectorGenerationAlgorithm::Run(const PositionVector &x_E) {
    // orientation is stored as a forwards (camera → equatorial) rotation, so apply it
    // directly to map x_E from the camera frame into the equatorial frame.
    return -(this->orientation * x_E);
}

}  // namespace found
