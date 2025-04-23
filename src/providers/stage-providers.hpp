#ifndef STAGE_PROVIDERS_H_
#define STAGE_PROVIDERS_H_

#include <memory>

#include "command-line/parsing/options.hpp"

#include "common/pipeline.hpp"

#include "calibrate/calibrate.hpp"

#include "distance/edge.hpp"
#include "distance/distance.hpp"
#include "distance/vectorize.hpp"

// TODO(nguy8tri): Include statement for Orbit Pipeline

#include "common/decimal.hpp"

namespace found {

/**
 * Provides a CalibrationAlgorithm
 * 
 * @param options The options to derive the calibration algorithm from
 * 
 * @return A pointer to the CalibrationAlgorithm
 */
std::unique_ptr<CalibrationAlgorithm> ProvideCalibrationAlgorithm(CalibrationOptions &options) {
    return std::make_unique<LOSTCalibrationAlgorithm>();
}

/**
 * Provides an EdgeDetectionAlgorithm
 * 
 * @param options The options to derive the edge detection algorithm from
 * 
 * @return std::unique_ptr<EdgeDetectionAlgorithm> The edge detection algorithm
 */
std::unique_ptr<EdgeDetectionAlgorithm> ProvideEdgeDetectionAlgorithm(DistanceOptions &options) {
    return std::make_unique<SimpleEdgeDetectionAlgorithm>();
}

/**
 * Provides a DistanceDeterminationAlgorithm
 * 
 * @param options The options to derive the distance determination algorithm from
 * 
 * @return std::unique_ptr<DistanceDeterminationAlgorithm> The distance determination algorithm
 */
std::unique_ptr<DistanceDeterminationAlgorithm> ProvideDistanceDeterminationAlgorithm(DistanceOptions &options) {
    return std::make_unique<SphericalDistanceDeterminationAlgorithm>(DECIMAL_M_R_E);
}

/**
 * Provides a VectorGenerationAlgorithm
 * 
 * @param options The options to derive the vector generation algorithm from
 * 
 * @return std::unique_ptr<VectorGenerationAlgorithm> The vector generation algorithm
 */
std::unique_ptr<VectorGenerationAlgorithm> ProvideVectorGenerationAlgorithm(DistanceOptions &options) {
    Quaternion relativeOrientation = SphericalToQuaternion(options.relOrientation);
    Quaternion referenceOrientation = SphericalToQuaternion(options.refOrientation);
    // TODO(nguy8tri) | TODO(ozbot11): If the relative Orientation is 0, then attempt to parse it out of the file
    if (options.refAsOrientation) {
        return std::make_unique<LOSTVectorGenerationAlgorithm>(referenceOrientation);
    }
    return std::make_unique<LOSTVectorGenerationAlgorithm>(relativeOrientation, referenceOrientation);
}

// TODO(nguy8tri): Include functions for providing the orbit pipeline stages

}  // namespace found

#endif  // STAGE_PROVIDERS_H_
