#include "calibrate/calibrate.hpp"

#include <utility>

#include "common/spatial/attitude-utils.hpp"

namespace found {

Quaternion LOSTCalibrationAlgorithm::Run(const std::pair<EulerAngles, EulerAngles> &orientations) {
    return SphericalToQuaternion(orientations.first) * SphericalToQuaternion(orientations.second);
}

}  // namespace found
