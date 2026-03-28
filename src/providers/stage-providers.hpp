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
FOUND_UNIQUE_PTR_TYPE(LOSTCalibrationAlgorithm, 1) ProvideCalibrationAlgorithm([[maybe_unused]]
                                                                               const CalibrationOptions &options) {
    static FOUND_POOL(LOSTCalibrationAlgorithm, 1) pool;
    return FOUND_UNIQUE_PTR(LOSTCalibrationAlgorithm, 1, pool);
}

/**
 * Provides an EdgeDetectionAlgorithm
 * 
 * @param options The options to derive the edge detection algorithm from
 * 
 * @return std::unique_ptr<EdgeDetectionAlgorithm> The edge detection algorithm
 */
FOUND_UNIQUE_PTR_TYPE(SimpleEdgeDetectionAlgorithm, 1) ProvideEdgeDetectionAlgorithm(const DistanceOptions &options) {
    static FOUND_POOL(SimpleEdgeDetectionAlgorithm, 1) pool;
    return FOUND_UNIQUE_PTR(SimpleEdgeDetectionAlgorithm, 1, pool, options.SEDAThreshold,
                            options.SEDABorderLen, options.SEDAOffset);
}

/**
 * Provides a DistanceDeterminationAlgorithm
 * 
 * @param options The options to derive the distance determination algorithm from
 * 
 * @return std::unique_ptr<DistanceDeterminationAlgorithm> The distance determination algorithm
 */
FOUND_UNIQUE_PTR_TYPE(SphericalDistanceDeterminationAlgorithm, 1)
ProvideDistanceDeterminationAlgorithm(const DistanceOptions &options) {
    static FOUND_POOL(SphericalDistanceDeterminationAlgorithm, 1) poolSDDA;
    static FOUND_POOL(IterativeSphericalDistanceDeterminationAlgorithm, 1) poolISDDA;
    if (options.distanceAlgo == SDDA) {
        return FOUND_UNIQUE_PTR(SphericalDistanceDeterminationAlgorithm, 1, poolSDDA, options.radius,
                                                                         Camera(options.focalLength,
                                                                                options.pixelSize,
                                                                                options.image.width,
                                                                                options.image.height));
    } else if (options.distanceAlgo == ISDDA) {
        return FOUND_UNIQUE_PTR(IterativeSphericalDistanceDeterminationAlgorithm, 1, poolISDDA, options.radius,
                                                                                  Camera(options.focalLength,
                                                                                         options.pixelSize,
                                                                                         options.image.width,
                                                                                         options.image.height),
                                                                                  options.ISDDAMinIters,
                                                                                  options.ISDDAMaxRefresh,
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
FOUND_UNIQUE_PTR_TYPE(LOSTVectorGenerationAlgorithm, 1)
ProvideVectorGenerationAlgorithm(const DistanceOptions &options) {
    Quaternion referenceOrientation = SphericalToQuaternion(options.refOrientation);
    static FOUND_POOL(LOSTVectorGenerationAlgorithm, 1) pool;
    if (options.calibrationData.header.version != emptyDFVer) {
        LOG_INFO("Using DataFile for calibration information");
        return FOUND_UNIQUE_PTR(LOSTVectorGenerationAlgorithm, 1, pool, options.calibrationData.relative_attitude,
                                                               referenceOrientation);
    } else {
        Quaternion relativeOrientation = SphericalToQuaternion(options.relOrientation);
        if (options.refAsOrientation) {
            LOG_INFO("Using provided reference orientation for calibration information");
            return FOUND_UNIQUE_PTR(LOSTVectorGenerationAlgorithm, 1, pool, referenceOrientation);
        }
        return FOUND_UNIQUE_PTR(LOSTVectorGenerationAlgorithm, 1, pool, relativeOrientation, referenceOrientation);
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
