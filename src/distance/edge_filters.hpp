#ifndef SRC_DISTANCE_EDGE_FILTERS_HPP_
#define SRC_DISTANCE_EDGE_FILTERS_HPP_

#include <memory>
#include <optional>

#include "command-line/parsing/options.hpp"
#include "common/pipeline/pipelines.hpp"
#include "distance/edge.hpp"

namespace found {


using EdgeFilteringAlgorithms = ModifyingPipeline<Points>;

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
 * A trivial implementation to ensure linkage / prevent undefined references.
 * Does nothing to the Points (no-op filter).
 */
class NoOpEdgeFilter : public EdgeFilteringAlgorithm {
 public:
    NoOpEdgeFilter() = default;
    ~NoOpEdgeFilter() override = default;

    void Run(Points &pts) override {
        // intentionally no-op
        (void)pts;
    }
};

/**
 * Convenience provider (no-arg). Returns a pipeline that contains a single
 * NoOpEdgeFilter so callers that expect a ready pipeline can use this.
 */
inline EdgeFilteringAlgorithms ProvideEdgeFilteringAlgorithm() {
    EdgeFilteringAlgorithms pipeline;
    static NoOpEdgeFilter noop;
    pipeline.Complete(noop);
    return pipeline;
}

/**
 * Options-aware provider. Constructs a pipeline containing all filters
 * that are enabled in options and returns it. If no filter is enabled,
 * returns std::nullopt to indicate "no filters".
 *
 * Note: stages added are function-local static instances so the pipeline
 * can safely store raw pointers to them.
 */
inline std::optional<EdgeFilteringAlgorithms> ProvideEdgeFilteringAlgorithm(const DistanceOptions &options) {
    EdgeFilteringAlgorithms pipeline;
    bool added = false;

    static NoOpEdgeFilter noop;  // safe persistent instance
    if (options.enableNoOpEdgeFilter) {
        pipeline.Complete(noop);
        added = true;
    }

    if (!added) return std::nullopt;
    return pipeline;
}

}  // namespace found

#endif  // SRC_DISTANCE_EDGE_FILTERS_HPP_
