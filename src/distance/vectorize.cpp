#include "distance/vectorize.hpp"

namespace found {

PositionVector LOSTVectorGenerationAlgorithm::Run(const PositionVector &x_E) {
    // Calculate the position vector using the orientation
    return -this->orientation.Rotate(x_E);
}

}  // namespace found
