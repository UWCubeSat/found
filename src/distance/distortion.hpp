#ifndef SRC_DISTANCE_DISTORTION_HPP_
#define SRC_DISTANCE_DISTORTION_HPP_

#include <memory>
#include <functional>

#include "common/style.hpp"
#include "common/pipeline/stages.hpp"

namespace found {

/**
 * The DistortionCorrection Algorithm class holds the distoriton correction algorithms. This 
 * algorithm takes in an edge of Earth calculated by an EdgeDetection Algorithm and adjusts it
 * to account for camera and lens distortion.
 * */
class DistortionCorrectionAlgorithm : public ModifyingStage<Points> {};

/**
 * Corrects distortion according to the Tsai camera model from this paper: https://doi.org/10.1109/JRA.1987.1087109
 * A simplified explanation of the formulas can be found here: https://stereopipeline.readthedocs.io/en/latest/pinholemodels.html
 * Well tested, highly used algorithm for correcting radial and tangential distortion.
 * 
 * Makes use of three radial distortion coefficients and two tangential distortion coefficients
 * */
class TsaiDistortionAlgorithm : public DistortionCorrectionAlgorithm {
 public:
    /**
     * k1, k2, k3 are radial distortion coefficients, determined by measuring the camera intrinsics
     * p1, p2 are tangential distortion coefficients, also determined from camera intrinsics
     * */
    TsaiDistortionAlgorithm(decimal k1, decimal k2, decimal k3, decimal p1, decimal p2, Camera &&cam) : 
        radialCoefficients_(Vec3(k1, k2, k3)),
        tangentialCoefficients_(Vec2(p1, p2)),
        cam_(cam) {}
      
    ~TsaiDistortionAlgorithm() {}

    /**
     * Place documentation here. Press enter to automatically make a new line
     * 
     * @param points The edge points to process
     * */
    void Run(const Points &points) override;
 private:
    // useful fields specific to this algorithm and helper methods
};

} // namespace found

#endif  // SRC_DISTANCE_DISTORTION_HPP_