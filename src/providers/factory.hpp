#ifndef SRC_PROVIDERS_FACTORY_HPP_
#define SRC_PROVIDERS_FACTORY_HPP_

#include <memory>
#include <utility>

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
inline CalibrationPipelineExecutorPtr CreateCalibrationPipelineExecutor(CalibrationOptions options) {
    static pool<CalibrationPipelineExecutor, 1> pool;  // GCOVR_EXCL_BR_LINE
    return make_unique<CalibrationPipelineExecutor, 1>(pool, std::move(options),
                                                       ProvideCalibrationAlgorithm(options));  // GCOVR_EXCL_BR_LINE
}

/**
 * Creates a DistancePipelineExecutor
 * 
 * @param options The options to create the pipeline executor from
 * 
 * @return A pointer to a DistancePipelineExecutor
 */
inline DistancePipelineExecutorPtr CreateDistancePipelineExecutor(DistanceOptions options) {
    static pool<DistancePipelineExecutor, 1> pool;  // GCOVR_EXCL_BR_LINE
    return make_unique<DistancePipelineExecutor, 1>(pool, std::move(options), ProvideEdgeDetectionAlgorithm(options),
                                                    ProvideDistanceDeterminationAlgorithm(options),
                                                    ProvideVectorGenerationAlgorithm(options));  // GCOVR_EXCL_BR_LINE
}

// TODO: Uncomment when orbit stage is implemented
/**
 * Creates an OrbitPipelineExecutor
 * 
 * @param options The options to create the pipeline executor from
 * 
 * @return A OrbitPipelineExecutor
 */
// inline std::unique_ptr<OrbitPipelineExecutor> CreateOrbitPipelineExecutor(OrbitOptions &&options) {
//     return std::make_unique<OrbitPipelineExecutor>(std::forward<OrbitOptions>(options),
//                                     ProvideOrbitPropagationAlgorithm(std::forward<OrbitOptions>(options)));
// }

}  // namespace found

#endif  // SRC_PROVIDERS_FACTORY_HPP_
