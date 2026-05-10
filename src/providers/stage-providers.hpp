#ifndef SRC_PROVIDERS_STAGE_PROVIDERS_HPP_
#define SRC_PROVIDERS_STAGE_PROVIDERS_HPP_

#include <memory>
#include <utility>

#include "command-line/parsing/options.hpp"

#include "common/pipeline/pipelines.hpp"

#include "calibrate/calibrate.hpp"

#include "distance/edge.hpp"
#include "distance/distance.hpp"
#include "distance/vectorize.hpp"
#include "distance/edge-filters.hpp"

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
inline cnt::unique_ptr<CalibrationAlgorithm> ProvideCalibrationAlgorithm(
    [[maybe_unused]] const CalibrationOptions &&options) {
    return cnt::make_unique_as<CalibrationAlgorithm, LOSTCalibrationAlgorithm>();
}

/**
 * Provides an EdgeDetectionAlgorithm
 * 
 * @param options The options to derive the edge detection algorithm from
 * 
 * @return cnt::unique_ptr<EdgeDetectionAlgorithm> The edge detection algorithm
 */
inline cnt::unique_ptr<EdgeDetectionAlgorithm> ProvideEdgeDetectionAlgorithm(const DistanceOptions &&options) {
    return cnt::make_unique_as<EdgeDetectionAlgorithm, SimpleEdgeDetectionAlgorithm>(
                                                                                     options.SEDAThreshold,
                                                                                     options.SEDABorderLen,
                                                                                     options.SEDAOffset);
}

/**
 * Provides a DistanceDeterminationAlgorithm
 * 
 * @param options The options to derive the distance determination algorithm from
 * 
 * @return cnt::unique_ptr<DistanceDeterminationAlgorithm> The distance determination algorithm
 */
inline cnt::unique_ptr<DistanceDeterminationAlgorithm> ProvideDistanceDeterminationAlgorithm(
    const DistanceOptions &&options) {
    if (options.distanceAlgo == SDDA) {
        return cnt::make_unique_as<DistanceDeterminationAlgorithm, SphericalDistanceDeterminationAlgorithm>(
            options.radius, Camera(options.focalLength, options.pixelSize, options.image.width,
                                   options.image.height));
    } else if (options.distanceAlgo == ISDDA) {
        return cnt::make_unique_as<DistanceDeterminationAlgorithm, IterativeSphericalDistanceDeterminationAlgorithm>(
            options.radius, Camera(options.focalLength, options.pixelSize, options.image.width,
                                   options.image.height), options.ISDDAMinIters, options.ISDDAMaxRefresh,
            options.ISDDADistRatio, options.ISDDADiscimRatio, options.ISDDAPdfOrd, options.ISDDARadLossOrd);
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
 * @return cnt::unique_ptr<VectorGenerationAlgorithm> The vector generation algorithm
 */
inline cnt::unique_ptr<VectorGenerationAlgorithm> ProvideVectorGenerationAlgorithm(const DistanceOptions &&options) {
    Quaternion referenceOrientation = SphericalToQuaternion(options.refOrientation);
    if (options.calibrationData.header.version != emptyDFVer) {
        LOG_INFO("Using DataFile for calibration information");
        return cnt::make_unique_as<VectorGenerationAlgorithm, LOSTVectorGenerationAlgorithm>(
            options.calibrationData.relative_attitude, referenceOrientation);
    } else {
        Quaternion relativeOrientation = SphericalToQuaternion(options.relOrientation);
        if (options.refAsOrientation) {
            LOG_INFO("Using provided reference orientation for calibration information");
            return cnt::make_unique_as<VectorGenerationAlgorithm, LOSTVectorGenerationAlgorithm>(
                referenceOrientation);
        }
        return cnt::make_unique_as<VectorGenerationAlgorithm, LOSTVectorGenerationAlgorithm>(
            relativeOrientation, referenceOrientation);
    }
}

/**
 * Provides an EdgeFilteringAlgorithms ptr. Currently only
 * allows no operations.
 * 
 * @param options The options to derive the edge filtering algorithm from
 * 
 * @return cnt::unique_ptr<EdgeFilteringAlgorithms> The edge filtering algorithm
 */
inline cnt::unique_ptr<EdgeFilteringAlgorithms> ProvideEdgeFilteringAlgorithm(const DistanceOptions &&options) {
    cnt::unique_ptr<EdgeFilteringAlgorithms> pipeline = cnt::make_unique<EdgeFilteringAlgorithms>();
    bool added = false;

    if (options.enableNoOpEdgeFilter) {
        pipeline->Complete(cnt::make_unique_as<ModifyingStage<Points>, NoOpEdgeFilter>());
        added = true;
    }

    if (!added) return cnt::unique_ptr<EdgeFilteringAlgorithms>();
    return pipeline;
}

// TODO: Uncomment when orbit stage is implemented
/**
 * Provides an OrbitPropagationAlgorithm
 * 
 * @param options The options to derive the orbit propagation algorithm from
 * 
 * @return std::unique_ptr<OrbitPropagationAlgorithm> The orbit propagation algorithm
 */
// std::unique_ptr<OrbitPropagationAlgorithm> ProvideOrbitPropagationAlgorithm(const OrbitOptions &options) {
//     return std::make_unique<ApproximateOrbitPropagationAlgorithm>(options.totalTime,
//                                                                   options.dt,
//                                                                   options.radius,
//                                                                   options.mu);
// }

}  // namespace found

#endif  // SRC_PROVIDERS_STAGE_PROVIDERS_HPP_
