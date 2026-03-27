#ifndef SRC_DISTANCE_EDGE_FILTERS_HPP_
#define SRC_DISTANCE_EDGE_FILTERS_HPP_

#include <memory>
#include <optional>
#include <utility>

#include "command-line/parsing/options.hpp"
#include "common/pipeline/pipelines.hpp"
#include "distance/edge.hpp"

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
};

/**
 * NoOpEdgeFilter
 *
 * A ModifyingStage implementation that performs no modifications
 * to the Points. This exists to provide a valid stage instance that can be
 * used by providers when a no-op filter is requested. This will not be needed
 * once an EdgeFilteringAlgorithm is implemented.
 */
class NoOpEdgeFilter : public ModifyingStage<Points> {
 public:
    NoOpEdgeFilter() = default;
    ~NoOpEdgeFilter() override = default;

    /**
     * Run
     *
     * No-op filter: intentionally does not modify the provided points.
     *
     * @param pts The Points to (not) modify.
     */
    void Run(Points &pts) override {
        // intentionally no-op
        (void)pts;
    }
};

}  // namespace found

#endif  // SRC_DISTANCE_EDGE_FILTERS_HPP_
