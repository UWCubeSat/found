#ifndef SRC_CALIBRATE_CALIBRATE_HPP_
#define SRC_CALIBRATE_CALIBRATE_HPP_

#include <utility>

#include "common/pipeline/stages.hpp"
#include "common/spatial/attitude-utils.hpp"

namespace found {

/**
 * The CalibrationAlgorithm is an interface for
 * algorithms that calibrate our orientation to the
 * reference orientation
 */
class CalibrationAlgorithm : public FunctionStage<std::pair<EulerAngles, EulerAngles>, Quaternion> {
 public:
    // Constructs this
    CalibrationAlgorithm() = default;
    // Destroys this
    virtual ~CalibrationAlgorithm() = default;
};

/**
 * The LostCalibrationAlgorithm class houses the calibration algorithm that uses
 * the orientation information from LOST to calibrate the camera's
 * local orientation with the reference orientation (i.e. LOST's camera)
 */
class LOSTCalibrationAlgorithm : public CalibrationAlgorithm {
 public:
    // Constructs this
    LOSTCalibrationAlgorithm() = default;
    /**
     * Runs the calibration algorithm
     * 
     * @param orientationF The orientation of reference frame F in terms of G in spherical coordinates.
     * @param orientationL The orientation of reference frame L in terms of G in spherical coordinates.
     * 
     * @return The quaternion that represents the rotation from reference frame F to reference frame L
     * 
     * @note Set the reference orientation to 0 to make this absolute (i.e., 
     * the orientation of this camera becomes the relative rotation)
     * 
     * @pre orientationF and orientationL must be respective to the same reference frame G
     */
    Quaternion Run(const EulerAngles &orientationF, const EulerAngles &orientationL) override;
};

}  // namespace found

#endif  // SRC_CALIBRATE_CALIBRATE_HPP_
