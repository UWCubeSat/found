#include "calibrate/calibrate.hpp"

#include <utility>

#include "common/spatial/attitude-utils.hpp"

namespace found {

Quaternion LOSTCalibrationAlgorithm::Run(const EulerAngles &orientationF, const EulerAngles &orientationL) {
    return SphericalToQuaternion(orientationF).conjugate() * SphericalToQuaternion(orientationL);
}

}  // namespace found
