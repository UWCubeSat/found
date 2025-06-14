#ifndef STAGE_PROVIDERS_H_
#define STAGE_PROVIDERS_H_

#include <memory>

#include "command-line/parsing/options.hpp"

#include "common/pipeline.hpp"

#include "calibrate/calibrate.hpp"

#include "distance/edge.hpp"
#include "distance/distance.hpp"
#include "distance/vectorize.hpp"

#include "orbit/orbit.hpp"

// TODO(nguy8tri): Include statement for Orbit Pipeline
// TODO: Fully Implement this after everything is well defined

#include "common/decimal.hpp"

namespace found {

/**
 * Provides a CalibrationAlgorithm
 * 
 * @param options The options to derive the calibration algorithm from
 * 
 * @return A pointer to the CalibrationAlgorithm
 */
std::unique_ptr<CalibrationAlgorithm> ProvideCalibrationAlgorithm([[maybe_unused]] CalibrationOptions &&options) {
    return std::make_unique<LOSTCalibrationAlgorithm>();
}

/**
 * Provides an EdgeDetectionAlgorithm
 * 
 * @param options The options to derive the edge detection algorithm from
 * 
 * @return std::unique_ptr<EdgeDetectionAlgorithm> The edge detection algorithm
 */
std::unique_ptr<EdgeDetectionAlgorithm> ProvideEdgeDetectionAlgorithm([[maybe_unused]] DistanceOptions &&options) {
    return std::make_unique<SimpleEdgeDetectionAlgorithm>(options.SEDAThreshold,
                                                          options.SEDABorderLen,
                                                          options.SEDAOffset);
}

/**
 * Provides a DistanceDeterminationAlgorithm
 * 
 * @param options The options to derive the distance determination algorithm from
 * 
 * @return std::unique_ptr<DistanceDeterminationAlgorithm> The distance determination algorithm
 */
std::unique_ptr<DistanceDeterminationAlgorithm> ProvideDistanceDeterminationAlgorithm(
                                                        [[maybe_unused]] DistanceOptions &&options) {
    return std::make_unique<SphericalDistanceDeterminationAlgorithm>(options.radius,
                                                                     Camera(options.focalLength,
                                                                            options.pixelSize,
                                                                            options.image.width,
                                                                            options.image.height));
}

/**
 * Provides a VectorGenerationAlgorithm
 * 
 * @param options The options to derive the vector generation algorithm from
 * 
 * @return std::unique_ptr<VectorGenerationAlgorithm> The vector generation algorithm
 */
std::unique_ptr<VectorGenerationAlgorithm> ProvideVectorGenerationAlgorithm(DistanceOptions &&options) {
    Quaternion relativeOrientation = SphericalToQuaternion(options.relOrientation);
    Quaternion referenceOrientation = SphericalToQuaternion(options.refOrientation);
    // TODO: If the relative Orientation is 0, then attempt to parse it out of the file
    if (options.refAsOrientation) {
        return std::make_unique<LOSTVectorGenerationAlgorithm>(referenceOrientation);
    }
    return std::make_unique<LOSTVectorGenerationAlgorithm>(relativeOrientation, referenceOrientation);
}

// TODO: Uncomment when orbit stage is implemented
/**
 * Provides an OrbitPropagationAlgorithm
 * 
 * @param options The options to derive the orbit propagation algorithm from
 * 
 * @return std::unique_ptr<OrbitPropagationAlgorithm> The orbit propagation algorithm
 */
// std::unique_ptr<OrbitPropagationAlgorithm> ProvideOrbitPropagationAlgorithm(OrbitOptions &&options) {
//     return std::make_unique<ApproximateOrbitPropagationAlgorithm>(options.totalTime,
//                                                                   options.dt,
//                                                                   options.radius,
//                                                                   options.mu);
// }

}  // namespace found

#endif  // STAGE_PROVIDERS_H_
