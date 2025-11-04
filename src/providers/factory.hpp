#ifndef SRC_PROVIDERS_FACTORY_HPP_
#define SRC_PROVIDERS_FACTORY_HPP_

#include <memory>
#include <utility>
#include <optional>

#include "command-line/execution/executors.hpp"
#include "providers/stage-providers.hpp"
#include "distance/edge-filters.hpp"  // ProvideEdgeFilteringAlgorithm

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
    // Read enable flags before anything that might move resources.
    bool enableNoop = options.enableNoOpEdgeFilter;

    // Call providers with const reference to options (no copies).
    auto edgeAlg = ProvideEdgeDetectionAlgorithm(options);
    auto distAlg = ProvideDistanceDeterminationAlgorithm(options);
    auto vecAlg = ProvideVectorGenerationAlgorithm(options);

    // Options-aware filters provider: returns optional pipeline if any filters enabled
    std::optional<EdgeFilteringAlgorithms> filtersOpt;
    if (enableNoop) {
        filtersOpt = ProvideEdgeFilteringAlgorithm(options);  // checks enableNoOpEdgeFilter
    }

    if (filtersOpt.has_value()) {
        return std::make_unique<DistancePipelineExecutor>(std::move(options),
                                    std::move(edgeAlg),
                                    std::move(distAlg),
                                    std::move(vecAlg),
                                    std::move(filtersOpt.value()));
    } else {
        return std::make_unique<DistancePipelineExecutor>(std::move(options),
                                    std::move(edgeAlg),
                                    std::move(distAlg),
                                    std::move(vecAlg));
    }
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
