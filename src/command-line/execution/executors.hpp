#ifndef SRC_COMMAND_LINE_EXECUTION_EXECUTORS_HPP_
#define SRC_COMMAND_LINE_EXECUTION_EXECUTORS_HPP_

#include <memory>

#include "common/pipeline/pipelines.hpp"
#include "command-line/parsing/options.hpp"
#include "common/style.hpp"

#include "calibrate/calibrate.hpp"

#include "distance/edge.hpp"
#include "distance/distance.hpp"
#include "distance/vectorize.hpp"
#include "distance/edge-filters.hpp"

#include "orbit/orbit.hpp"

namespace found {

/**
 * PipelineExecutor is an interface for classes
 * that execute complex pipelines.
 */
class PipelineExecutor {
 public:
    /// Destroys this
    virtual ~PipelineExecutor() = default;
    /**
     * Executes the relavent Pipeline
     */
    virtual void ExecutePipeline() = 0;
    /**
     * Outputs the results of the Pipeline
     * in some format
     */
    virtual void OutputResults() = 0;
};

/**
 * CalibrationPipelineExecutor is the pipeline
 * executor for the calibration pipeline.
 */
class CalibrationPipelineExecutor : public PipelineExecutor {
 public:
    /**
     * Constructs a CalibrationPipelineExecutor
     * 
     * @param options The options to create the pipeline
     * @param calibrationAlgorithm The calibration algorithm to use
     */
    explicit CalibrationPipelineExecutor(CalibrationOptions &&options,
                                         std::unique_ptr<CalibrationAlgorithm> calibrationAlgorithm);

    void ExecutePipeline() override;
    void OutputResults() override;

 private:
    /// The Calibration options being used
    const CalibrationOptions options_;
    /// The Calibration pipeline
    CalibrationPipeline pipeline_;
};

/**
 * DistancePipelineExecutor is the pipeline
 * executor for the distance determination pipeline.
 */
class DistancePipelineExecutor : public PipelineExecutor {
 public:
    /**
     * Destroys this and all distance determination pipeline resources
     */
    ~DistancePipelineExecutor();

    /**
     * Constructs a DistancePipelineExecutor (no edge-filters)
     *
     * @param options The DistanceOptions to configure the pipeline
     * @param edgeDetectionAlgorithm The EdgeDetectionAlgorithm used by the pipeline (moved into the executor)
     * @param distanceAlgorithm The DistanceDeterminationAlgorithm used by the pipeline (moved into the executor)
     * @param vectorizationAlgorithm The VectorGenerationAlgorithm used by the pipeline (moved into the executor)
     *
     * @pre edgeDetectionAlgorithm, distanceAlgorithm, and vectorizationAlgorithm are non-null and already
     *      configured to operate on (Image -> Points -> PositionVector) in that order.
     * @pre Each provided stage is already "ready" (e.g., pipelines passed in were Completed) before transfer.
     */
    explicit DistancePipelineExecutor(DistanceOptions &&options,
                                      std::unique_ptr<EdgeDetectionAlgorithm> edgeDetectionAlgorithm,
                                      std::unique_ptr<DistanceDeterminationAlgorithm> distanceAlgorithm,
                                      std::unique_ptr<VectorGenerationAlgorithm> vectorizationAlgorithm);

    /**
     * Constructs a DistancePipelineExecutor with an edge-filtering pipeline
     *
     * @param options The DistanceOptions to configure the pipeline (moved into the executor)
     * @param edgeDetectionAlgorithm The EdgeDetectionAlgorithm used by the pipeline (moved into the executor)
     * @param filters A pipeline of edge filtering stages; ownership is transferred to the executor
     * @param distanceAlgorithm The DistanceDeterminationAlgorithm used by the pipeline (moved into the executor)
     * @param vectorizationAlgorithm The VectorGenerationAlgorithm used by the pipeline (moved into the executor)
     *
     * @pre edgeDetectionAlgorithm, filters, distanceAlgorithm, and vectorizationAlgorithm are non-null.
     * @pre filters has been completed (ready) prior to being passed in so it can run as a stage.
     * @pre Stage input/output types align with the Distance pipeline: Image -> Points -> Points -> PositionVector.
     */
    explicit DistancePipelineExecutor(DistanceOptions &&options,
                                      std::unique_ptr<EdgeDetectionAlgorithm> edgeDetectionAlgorithm,
                                      std::unique_ptr<EdgeFilteringAlgorithms> filters,
                                      std::unique_ptr<DistanceDeterminationAlgorithm> distanceAlgorithm,
                                      std::unique_ptr<VectorGenerationAlgorithm> vectorizationAlgorithm);

    void ExecutePipeline() override;
    void OutputResults() override;

 private:
    /// The DistanceOptions being used
    const DistanceOptions options_;
    /// The Distance pipeline being used
    DistancePipeline pipeline_;
};

/**
 * OrbitPipelineExecutor is the pipeline
 * executor for the orbit determination pipeline.
 */
class OrbitPipelineExecutor : public PipelineExecutor {
 public:
    /**
     * Constructs a OrbitPipelineExecutor
     * 
     * @param options The options to create the pipeline
     * @param orbitPropagationAlgorithm The orbit propagation algorithm to use
     */
    explicit OrbitPipelineExecutor(OrbitOptions &&options,
                                   std::unique_ptr<OrbitPropagationAlgorithm> orbitPropagationAlgorithm);

    void ExecutePipeline() override;
    void OutputResults() override;

 private:
    /// The Orbit options being used
    const OrbitOptions options_;
    /// The Orbit pipeline
    OrbitPipeline pipeline_;
};

}  // namespace found

#endif  // SRC_COMMAND_LINE_EXECUTION_EXECUTORS_HPP_
