#ifndef EXECUTOR_PROVIDERS_H
#define EXECUTOR_PROVIDERS_H

#include <memory>

#include "command-line/execution/executors.hpp"
#include "providers/stage-providers.hpp"

namespace found {

/**
 * Creates a CalibrationPipelineExecutor
 * 
 * @param options The options to create the pipeline executor from
 * 
 * @return A pointer to a CalibrationPipelineExecutor
 */
inline std::unique_ptr<CalibrationPipelineExecutor> CreateCalibrationPipelineExecutor(CalibrationOptions &options) {
    return std::make_unique<CalibrationPipelineExecutor>(options, ProvideCalibrationAlgorithm(options));
}

/**
 * Creates a DistancePipelineExecutor
 * 
 * @param options The options to create the pipeline executor from
 * 
 * @return A pointer to a DistancePipelineExecutor
 */
inline std::unique_ptr<DistancePipelineExecutor> CreateDistancePipelineExecutor(DistanceOptions &options) {
    return std::make_unique<DistancePipelineExecutor>(options,
                                    ProvideEdgeDetectionAlgorithm(options),
                                    ProvideDistanceDeterminationAlgorithm(options),
                                    ProvideVectorGenerationAlgorithm(options));
}

/**
 * Creates an OrbitPipelineExecutor
 * 
 * @param options The options to create the pipeline executor from
 * 
 * @return A OrbitPipelineExecutor
 */
inline std::unique_ptr<OrbitPipelineExecutor> CreateOrbitPipelineExecutor(OrbitOptions &options) {
    return std::make_unique<OrbitPipelineExecutor>(options,
                                    ProvideOrbitPropagationAlgorithm(options));
}

}  // namespace found

#endif  // EXECUTOR_PROVIDERS_H
