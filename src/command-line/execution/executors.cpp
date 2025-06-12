#include "command-line/execution/executors.hpp"

#include <memory>
#include <utility>
#include <cstring>

#include "common/logging.hpp"
#include "common/time/time.hpp"

namespace found {

CalibrationPipelineExecutor::CalibrationPipelineExecutor(CalibrationOptions &&options,
                                                         std::unique_ptr<CalibrationAlgorithm> calibrationAlgorithm)
                                                         : options_(std::move(options)) {
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
    // Output the results of the calibration
    Quaternion *&calibrationQuaternion = this->pipeline_.GetProduct();
    LOG_INFO("Calibration Quaternion: (" << calibrationQuaternion->real << ", "
                                         << calibrationQuaternion->i << ", "
                                         << calibrationQuaternion->j << ", "
                                         << calibrationQuaternion->k << ")");
    DataFile outputDF{};
    outputDF.relative_attitude = *calibrationQuaternion;
    std::ofstream outputFile(this->options_.outputFile);
    serializeDataFile(outputDF, outputFile);
}

DistancePipelineExecutor::~DistancePipelineExecutor() {
    stbi_image_free(this->options_.image.image);
}

DistancePipelineExecutor::DistancePipelineExecutor(DistanceOptions &&options,
                                                   std::unique_ptr<EdgeDetectionAlgorithm> edgeDetectionAlgorithm,
                                                   std::unique_ptr<DistanceDeterminationAlgorithm> distanceAlgorithm,
                                                   std::unique_ptr<VectorGenerationAlgorithm> vectorizationAlgorithm)
                                                   : options_(std::move(options)) {
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
    PositionVector *&positionVector = this->pipeline_.GetProduct();
    LOG_INFO("Calculated Position: (" << positionVector->x << ", "
                                      << positionVector->y << ", "
                                      << positionVector->z << ") m");
    LOG_INFO("Distance from Earth: " << positionVector->Magnitude() << " m");
    // TODO: Figure out a much more optimized way of doing this please, especially
    // since we're saving it into the exact same file, there should be an easy way
    // to simply modify the file directly instead of this mess.
    DataFile outputDF{};
    if (this->options_.calibrationData.header.version != emptyDFVer) {
        outputDF.header = this->options_.calibrationData.header;
        outputDF.relative_attitude = this->options_.calibrationData.relative_attitude;
        outputDF.positions = std::make_unique<LocationRecord[]>(outputDF.header.num_positions + 1);
        std::memcpy(outputDF.positions.get(),
                    this->options_.calibrationData.positions.get(),
                    outputDF.header.num_positions);
    } else {
        outputDF.relative_attitude = SphericalToQuaternion(this->options_.relOrientation);
        outputDF.positions = std::make_unique<LocationRecord[]>(1);
    }
    outputDF.positions[outputDF.header.num_positions++] = {static_cast<uint64_t>(getUT1Time().epochs), *positionVector};
    if (this->options_.outputFile != "") {
        std::ofstream outputFile(this->options_.outputFile);
        serializeDataFile(outputDF, outputFile);
    } else {
        std::ofstream outputFile(this->options_.calibrationData.path);
        serializeDataFile(outputDF, outputFile);
    }
}

OrbitPipelineExecutor::OrbitPipelineExecutor(OrbitOptions &&options,
                                             std::unique_ptr<OrbitPropagationAlgorithm> orbitPropagationAlgorithm)
                                             : options_(std::move(options)) {
    this->orbitPropagationAlgorithm = std::move(orbitPropagationAlgorithm);
    this->pipeline_.Complete(*this->orbitPropagationAlgorithm);
}

void OrbitPipelineExecutor::ExecutePipeline() {
    // Results are stored within the pipeline, can also be accessed
    // via this function call
    this->pipeline_.Run(this->options_.positionData);
}

void OrbitPipelineExecutor::OutputResults() {
    // TODO: Output this somewhere
    [[maybe_unused]] LocationRecord &futurePosition = this->pipeline_.GetProduct()->back();
    LOG_INFO("Calculated Future Position: (" << futurePosition.position.x << ", "
                                             << futurePosition.position.y << ", "
                                             << futurePosition.position.z << ") m"
                                             << " at time "
                                             << futurePosition.timestamp << " s");
}

}  // namespace found
