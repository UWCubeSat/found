#ifndef SRC_DISTANCE_EDGE_FILTERS_HPP_
#define SRC_DISTANCE_EDGE_FILTERS_HPP_

#include <memory>
#include <optional>
#include <utility>

#include "command-line/parsing/options.hpp"
#include "common/pipeline/pipelines.hpp"
#include "distance/edge.hpp"
#include "providers/stage-providers.hpp"

namespace found {


typedef ModifyingPipeline<Points> EdgeFilteringAlgorithms;

/**
 * Abstract superclass for edge-filtering algorithms.
 * modifies Points in-place (ModifyingStage<Points>).
 */
class EdgeFilteringAlgorithm : public ModifyingStage<Points> {
 public:
    EdgeFilteringAlgorithm() = default;
    virtual ~EdgeFilteringAlgorithm() = default;
    // Implementations override void Run(Points &)
};


/**
 * Options-aware provider. Constructs a pipeline containing all filters
 * that are enabled in options and returns it. If no filter is enabled,
 * returns nullptr to indicate "no filters".
 */
inline std::unique_ptr<EdgeFilteringAlgorithms> ProvideEdgeFilteringAlgorithm(const DistanceOptions &options) {
    std::unique_ptr<EdgeFilteringAlgorithms> pipeline = std::make_unique<EdgeFilteringAlgorithms>();
    bool added = false;

    if (options.enableNoOpEdgeFilter) {
        pipeline->Complete(detail::kNoOpEdgeFilter);
        added = true;
    }

    if (!added) return nullptr;
    return pipeline;
}

}  // namespace found

#endif  // SRC_DISTANCE_EDGE_FILTERS_HPP_
