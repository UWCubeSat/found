#ifndef PREPROCESSING_H_
#define PREPROCESSING_H_

#include <memory>

#include "common/pipeline.hpp"
#include "command-line/parsing/options.hpp"
#include "common/style.hpp"

#include "calibrate/calibrate.hpp"

#include "distance/edge.hpp"
#include "distance/distance.hpp"
#include "distance/vectorize.hpp"

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
    /// The Calibration Algorithm used
    std::unique_ptr<CalibrationAlgorithm> calibrationAlgorithm;
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
     * Constructs a DistancePipelineExecutor
     * 
     * @param options The options to create the pipeline
     * @param edgeDetectionAlgorithm The edge detection algorithm to use
     * @param distanceAlgorithm The distance determination algorithm to use
     * @param vectorizationAlgorithm The vectorization algorithm to use
     * 
     * @pre options.image.image must be point to heap allocated memory.
     * This is guarenteed as long as strtoimage is used to create the image,
     * and it throws an error if the image is not valid.
     */
    explicit DistancePipelineExecutor(const DistanceOptions &&options,
                                      std::unique_ptr<EdgeDetectionAlgorithm> edgeDetectionAlgorithm,
                                      std::unique_ptr<DistanceDeterminationAlgorithm> distanceAlgorithm,
                                      std::unique_ptr<VectorGenerationAlgorithm> vectorizationAlgorithm);

    void ExecutePipeline() override;
    void OutputResults() override;

 private:
    /// The DistanceOptions being used
    const DistanceOptions options_;
    /// The Distance pipeline being used
    DistancePipeline pipeline_;
    /// The Edge Detection Algorithm used
    std::unique_ptr<EdgeDetectionAlgorithm> edgeDetectionAlgorithm;
    /// The Distance Determination Algorithm being used
    std::unique_ptr<DistanceDeterminationAlgorithm> distanceAlgorithm;
    /// The Vectorization/Rotation Algorithm being used
    std::unique_ptr<VectorGenerationAlgorithm> vectorizationAlgorithm;
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
    explicit OrbitPipelineExecutor(const OrbitOptions &&options,
                                   std::unique_ptr<OrbitPropagationAlgorithm> orbitPropagationAlgorithm);

    void ExecutePipeline() override;
    void OutputResults() override;

 private:
    /// The Orbit options being used
    const OrbitOptions options_;
    /// The Orbit pipeline
    OrbitPipeline pipeline_;
    /// The Orbit Propagation Algorithm being used
    std::unique_ptr<OrbitPropagationAlgorithm> orbitPropagationAlgorithm;
};

}  // namespace found

#endif  // PREPROCESSING_H_
