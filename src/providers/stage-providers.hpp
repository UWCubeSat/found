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
inline unique_ptr<LOSTCalibrationAlgorithm, 1> ProvideCalibrationAlgorithm([[maybe_unused]]
                                                                               const CalibrationOptions &options) {
    static pool<LOSTCalibrationAlgorithm, 1> pool;  // GCOVR_EXCL_BR_LINE
    return make_unique<LOSTCalibrationAlgorithm, 1>(pool);
}

/**
 * Provides an EdgeDetectionAlgorithm
 * 
 * @param options The options to derive the edge detection algorithm from
 * 
 * @return std::unique_ptr<EdgeDetectionAlgorithm> The edge detection algorithm
 */
inline unique_ptr<SimpleEdgeDetectionAlgorithm, 1> ProvideEdgeDetectionAlgorithm(DistanceOptions &&options) {
    static pool<SimpleEdgeDetectionAlgorithm, 1> pool;  // GCOVR_EXCL_BR_LINE
    return make_unique<SimpleEdgeDetectionAlgorithm, 1>(pool, options.SEDAThreshold, options.SEDABorderLen,
                                                        options.SEDAOffset);
}

/**
 * Provides a DistanceDeterminationAlgorithm
 * 
 * @param options The options to derive the distance determination algorithm from
 * 
 * @return std::unique_ptr<DistanceDeterminationAlgorithm> The distance determination algorithm
 */
inline unique_ptr<SphericalDistanceDeterminationAlgorithm, 1>
ProvideDistanceDeterminationAlgorithm(DistanceOptions &&options) {
    static pool<SphericalDistanceDeterminationAlgorithm, 1> poolSDDA;  // GCOVR_EXCL_BR_LINE
    static pool<IterativeSphericalDistanceDeterminationAlgorithm, 1> poolISDDA;  // GCOVR_EXCL_BR_LINE
    if (options.distanceAlgo == SDDA) {
        return make_unique<SphericalDistanceDeterminationAlgorithm, 1>(poolSDDA, options.radius,
                                                                       Camera(options.focalLength,
                                                                              options.pixelSize,
                                                                              options.image.width,
                                                                              options.image.height));
    } else if (options.distanceAlgo == ISDDA) {
        return make_unique<IterativeSphericalDistanceDeterminationAlgorithm, 1>(poolISDDA, options.radius,
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
inline unique_ptr<LOSTVectorGenerationAlgorithm, 1>
ProvideVectorGenerationAlgorithm(DistanceOptions &&options) {
    Quaternion referenceOrientation = SphericalToQuaternion(options.refOrientation);
    static pool<LOSTVectorGenerationAlgorithm, 1> pool;  // GCOVR_EXCL_BR_LINE
    if (options.calibrationData.header.version != emptyDFVer) {
        LOG_INFO("Using DataFile for calibration information");
        return make_unique<LOSTVectorGenerationAlgorithm, 1>(pool, options.calibrationData.relative_attitude,
                                                             referenceOrientation);
    } else {
        Quaternion relativeOrientation = SphericalToQuaternion(options.relOrientation);
        if (options.refAsOrientation) {
            LOG_INFO("Using provided reference orientation for calibration information");
            return make_unique<LOSTVectorGenerationAlgorithm, 1>(pool, referenceOrientation);
        }
        return make_unique<LOSTVectorGenerationAlgorithm, 1>(pool, relativeOrientation, referenceOrientation);
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
