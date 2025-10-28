#ifndef SRC_DISTANCE_EDGE_FILTERS_HPP_
#define SRC_DISTANCE_EDGE_FILTERS_HPP_

#include <memory>

#include "common/pipeline/pipelines.hpp"
// including to use Points
#include "distance/edge.hpp" 

namespace found {

// pipeline type alias for edge-filtering stages operating on Points
using EdgeFilteringAlgorithms = ModifyingPipeline<Points>;

/**
 * Abstract superclass for edge-filtering algorithms.
 * This modifies Points in-place (ModifyingStage<Points>).
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
 * Had to use this to avoid memory leaks. Not sure if this is proper implementation
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
 * Provider that constructs an EdgeFilteringAlgorithms pipeline and
 * registers requested edge filters. For now it registers a single
 * NoOpEdgeFilter so builds succeed
 */
inline EdgeFilteringAlgorithms ProvideEdgeFilteringAlgorithm() {
    EdgeFilteringAlgorithms pipeline;
    // Use a function-local static object so we don't allocate with new and leak
    static NoOpEdgeFilter noop;
    pipeline.Complete(noop);
    return pipeline;
}

}  

#endif  // SRC_DISTANCE_EDGE_FILTERS_HPP_
