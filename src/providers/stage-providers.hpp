#ifndef SRC_PROVIDERS_STAGE_PROVIDERS_HPP_
#define SRC_PROVIDERS_STAGE_PROVIDERS_HPP_

#include <memory>

#include "command-line/parsing/options.hpp"

#include "common/pipeline/pipelines.hpp"

#include "calibrate/calibrate.hpp"

#include "distance/edge.hpp"
#include "distance/distance.hpp"
#include "distance/vectorize.hpp"

#include "orbit/orbit.hpp"

// TODO(nguy8tri): Include statement for Orbit Pipeline
// TODO: Fully Implement this when orbit stage is implemented

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
std::unique_ptr<EdgeDetectionAlgorithm> ProvideEdgeDetectionAlgorithm(DistanceOptions &&options) {
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
std::unique_ptr<DistanceDeterminationAlgorithm> ProvideDistanceDeterminationAlgorithm(DistanceOptions &&options) {
    if (options.distanceAlgo == SDDA) {
        return std::make_unique<SphericalDistanceDeterminationAlgorithm>(options.radius,
                                                                         Camera(options.focalLength,
                                                                                options.pixelSize,
                                                                                options.image.width,
                                                                                options.image.height));
    } else if (options.distanceAlgo == ISDDA) {
        return std::make_unique<IterativeSphericalDistanceDeterminationAlgorithm>(options.radius,
                                                                                  Camera(options.focalLength,
                                                                                         options.pixelSize,
                                                                                         options.image.width,
                                                                                         options.image.height),
                                                                                  options.ISDDAMinIters,
                                                                                  options.ISDDADistRatio,
                                                                                  options.ISDDADiscimRatio,
                                                                                  options.ISDDAPdfOrd,
                                                                                  options.ISDDARadLossOrd);
    } else {
        LOG_ERROR("Unrecognized distance algorithm: " << options.distanceAlgo);
        throw std::runtime_error("Unrecognized distance algorithm: " + options.distanceAlgo);
    }
}

/**
 * Provides a VectorGenerationAlgorithm
 * 
 * @param options The options to derive the vector generation algorithm from
 * 
 * @return std::unique_ptr<VectorGenerationAlgorithm> The vector generation algorithm
 */
std::unique_ptr<VectorGenerationAlgorithm> ProvideVectorGenerationAlgorithm(DistanceOptions &&options) {
    Quaternion referenceOrientation = SphericalToQuaternion(options.refOrientation);
    if (options.calibrationData.header.version != emptyDFVer) {
        LOG_INFO("Using DataFile for calibration information");
        return std::make_unique<LOSTVectorGenerationAlgorithm>(options.calibrationData.relative_attitude,
                                                               referenceOrientation);
    } else {
        Quaternion relativeOrientation = SphericalToQuaternion(options.relOrientation);
        if (options.refAsOrientation) {
            LOG_INFO("Using provided reference orientation for calibration information");
            return std::make_unique<LOSTVectorGenerationAlgorithm>(referenceOrientation);
        }
        return std::make_unique<LOSTVectorGenerationAlgorithm>(relativeOrientation, referenceOrientation);
    }
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

#endif  // SRC_PROVIDERS_STAGE_PROVIDERS_HPP_
