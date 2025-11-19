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
inline std::unique_ptr<CalibrationPipelineExecutor> CreateCalibrationPipelineExecutor(CalibrationOptions &&options) {
    return std::make_unique<CalibrationPipelineExecutor>(std::forward<CalibrationOptions>(options),
                                    ProvideCalibrationAlgorithm(std::forward<CalibrationOptions>(options)));
}

/**
 * Creates a DistancePipelineExecutor
 * 
 * @param options The options to create the pipeline executor from
 * 
 * @return A pointer to a DistancePipelineExecutor
 */
inline std::unique_ptr<DistancePipelineExecutor> CreateDistancePipelineExecutor(DistanceOptions &&options) {
    return std::make_unique<DistancePipelineExecutor>(std::forward<DistanceOptions>(options),
                                    ProvideEdgeDetectionAlgorithm(std::forward<DistanceOptions>(options)),
                                    ProvideDistanceDeterminationAlgorithm(std::forward<DistanceOptions>(options)),
                                    ProvideVectorGenerationAlgorithm(std::forward<DistanceOptions>(options)));
}

#ifdef TEST

/**
 * Creates a TestEdgePipelineExecutor
 * 
 * @param options The options to create the pipeline executor from
 * 
 * @return A pointer to a TestEdgePipelineExecutor
 */
inline std::unique_ptr<TestEdgePipelineExecutor> CreateTestEdgePipelineExecutor(DistanceOptions &&options) {
    return std::make_unique<TestEdgePipelineExecutor>(std::forward<DistanceOptions>(options),
                                    ProvideEdgeDetectionAlgorithm(std::forward<DistanceOptions>(options)));
}

/**
 * Creates a TestDistancePipelineExecutor
 * 
 * @param options The options to create the pipeline executor from
 * 
 * @return A pointer to a TestDistancePipelineExecutor
 */
inline std::unique_ptr<TestDistancePipelineExecutor> CreateTestDistancePipelineExecutor(DistanceOptions &&options) {
    return std::make_unique<TestDistancePipelineExecutor>(std::forward<DistanceOptions>(options),
                                    ProvideDistanceDeterminationAlgorithm(std::forward<DistanceOptions>(options)));
}

#endif

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
