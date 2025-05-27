#include "command-line/execution/executors.hpp"

#include <memory>
#include <utility>

#include "common/logging.hpp"

namespace found {

CalibrationPipelineExecutor::CalibrationPipelineExecutor(const CalibrationOptions &options,
                                                         std::unique_ptr<CalibrationAlgorithm> calibrationAlgorithm)
                                                         : options_(options) {
    this->calibrationAlgorithm = std::move(calibrationAlgorithm);
    this->pipeline_.Complete(*this->calibrationAlgorithm);
}

void CalibrationPipelineExecutor::ExecutePipeline() {
    // Results are stored within the pipeline, can also be accessed
    // via this function call
    this->pipeline_.Run({this->options_.lclOrientation,
                         this->options_.refOrientation});
}

void CalibrationPipelineExecutor::OutputResults() {
    // TODO: Write the output to the datafile
    // Output the results of the calibration
    [[maybe_unused]] Quaternion *&calibrationQuaternion = this->pipeline_.GetProduct();
    LOG_INFO("Calibration Quaternion: (" << calibrationQuaternion->real << ", "
                                         << calibrationQuaternion->i << ", "
                                         << calibrationQuaternion->j << ", "
                                         << calibrationQuaternion->k << ")");
}

DistancePipelineExecutor::~DistancePipelineExecutor() {
    stbi_image_free(this->options_.image.image);
}

DistancePipelineExecutor::DistancePipelineExecutor(const DistanceOptions &options,
                                                   std::unique_ptr<EdgeDetectionAlgorithm> edgeDetectionAlgorithm,
                                                   std::unique_ptr<DistanceDeterminationAlgorithm> distanceAlgorithm,
                                                   std::unique_ptr<VectorGenerationAlgorithm> vectorizationAlgorithm)
                                                   : options_(options) {
    this->edgeDetectionAlgorithm = std::move(edgeDetectionAlgorithm);
    this->distanceAlgorithm = std::move(distanceAlgorithm);
    this->vectorizationAlgorithm = std::move(vectorizationAlgorithm);
    this->pipeline_.AddStage(*this->edgeDetectionAlgorithm)
                   .AddStage(*this->distanceAlgorithm)
                   .Complete(*this->vectorizationAlgorithm);
}

void DistancePipelineExecutor::ExecutePipeline() {
    // Results are stored within the pipeline, can also be accessed
    // via this function call
    this->pipeline_.Run(this->options_.image);
}

void DistancePipelineExecutor::OutputResults() {
    // TODO: Write the output to the datafile
    [[maybe_unused]] PositionVector *&positionVector = this->pipeline_.GetProduct();
    LOG_INFO("Calculated Position: (" << positionVector->x << ", "
                                      << positionVector->y << ", "
                                      << positionVector->z << ") m");
    LOG_INFO("Distance from Earth: " << positionVector->Magnitude() << " m");
}

OrbitPipelineExecutor::OrbitPipelineExecutor(const OrbitOptions &options,
                                             std::unique_ptr<OrbitPropagationAlgorithm> orbitPropagationAlgorithm)
                                             : options_(options) {
    this->orbitPropagationAlgorithm = std::move(orbitPropagationAlgorithm);
    this->pipeline_.Complete(*this->orbitPropagationAlgorithm);
}

// TODO: Implement the ExecutePipeline and OutputResults methods for OrbitPipelineExecutor when
// the Data Serialization branch is merged

void OrbitPipelineExecutor::ExecutePipeline() {
    // Results are stored within the pipeline, can also be accessed
    // via this function call
    // this->pipeline_.Run();
}

void OrbitPipelineExecutor::OutputResults() {
}

}  // namespace found
