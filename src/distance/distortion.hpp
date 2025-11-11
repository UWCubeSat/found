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
 * Corrects radial distortion created by the fisheye lens of the FOUND camera. Particularly, 
 * applies the function 'insert function here' from 'paper'.
 * */
class RadialDistortionCorrectionAlgorithm : public DistortionCorrectionAlgorithm {
 public:
    /**
     * Will pass in flags for camera distortion k coefficients calculated pre-launch via standard
     * DistanceOptions mega-flag in pipeline
     * 
     * gamma = (1 + k_1 * r^2 + k_2 * r^4 + k_3 * r^6) / (1 + k_4 * r^2 + k_5 * r^4 + k_6 * r^6)
     * x_distorted = x_actual * gamma
     * y_distorted = y_actual * gamma
     * 
     * */
    RadialDistortionCorrectionAlgorithm(/*Put more fields here!*/);

    /**
     * Destroys this object
     * */
    virtual ~RadialDistortionCorrectionAlgorithm(/*Put more fields here!*/);

    /**
     * Place documentation here. Press enter to automatically make a new line
     * 
     * @param points The edge points to process
     * */
    void Run(const Points &points) override;
 private:
    // useful fields specific to this algorithm and helper methods
};

/**
 * Corrects tangential distortion created by any angle offset between camera and lens of the FOUND 
 * camera. Particularly, applies the function 'insert function here' from 'paper'.
 * */
class TangentialDistortionCorrectionAlgorithm : public DistortionCorrectionAlgorithm {
 public:
    /**
     * Will pass in flags for camera distortion p coefficients calculated pre-launch via standard
     * DistanceOptions mega-flag in pipeline
     * 
     * x_distorted = x + (2 * p_1 * x * y + p_2 * (r^2 + 2 * x^2))
     * y_distorted = y + (2 * p^2 * x * y + p_1 * (r^2 + 2 * y^2))
     * 
     * */
    TangentialDistortionCorrectionAlgorithm(/*Put more fields here!*/);

    /**
     * Destroys this object
     * */
    virtual ~TangentialDistortionCorrectionAlgorithm(/*Put more fields here!*/);

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