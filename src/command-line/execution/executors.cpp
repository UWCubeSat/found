#include "command-line/execution/executors.hpp"

#include <memory>
#include <utility>
#include <ctime>

#include "common/time/time.hpp"
#include "common/decimal.hpp"
#include "common/logging.hpp"

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

DistancePipelineExecutor::DistancePipelineExecutor(const DistanceOptions &&options,
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
    // TODO: Write the output to the datafile
    [[maybe_unused]] PositionVector *&positionVector = this->pipeline_.GetProduct();
    LOG_INFO("Calculated Position: (" << positionVector->x << ", "
                                      << positionVector->y << ", "
                                      << positionVector->z << ") m");
    LOG_INFO("Distance from Earth: " << positionVector->Magnitude() << " m");

    // I've placed the code here to output this in lat/long/alt, it is
    // up to you all to figure out how to use it. This code is untested,
    // at the time of writing it, so make sure you test it for accuracy.
    // One way you could actually do output of results in general though
    // is to make more Stage objects that just do that.

    // Converts epoch time to GMST in degrees, then we convert to radians
    // We should ensure Euclidean Mod, but both the divisor and dividend
    // are positive, so we don't need it (GMST > 0 after Jan 1st, 2000).
    decimal GMST = std::fmod(DECIMAL_M_PI * getCurrentGreenwichMeanSiderealTime() / 180.0, 2 * DECIMAL_M_PI);
    // Figure out Earth's Rotating Frame and express the position in that frame
    Quaternion toEarthRotatingFrame = SphericalToQuaternion(GMST, 0, 0);
    Vec3 position = toEarthRotatingFrame.Conjugate().Rotate(*positionVector);
    // Figure out the right ascension and declination of the vector
    decimal RA = std::atan2(position.y, position.x);  // Huh, the range is [-PI, PI], not [0, 2PI]. That's convenient
    decimal DE = std::asin(position.Normalize().z);  // Range is [-PI/2, PI/2]

    // Longitude, Lattitude and Altitude Follow, with conversion
    // to degrees and range adjustment from RA to longitude
    [[maybe_unused]] decimal longitude = 180.0 * RA / DECIMAL_M_PI;
    [[maybe_unused]] decimal latitude = 180 * DE / DECIMAL_M_PI;
    [[maybe_unused]] decimal altitude = position.Magnitude();
}

OrbitPipelineExecutor::OrbitPipelineExecutor(const OrbitOptions &&options,
                                             std::unique_ptr<OrbitPropagationAlgorithm> orbitPropagationAlgorithm)
                                             : options_(std::move(options)) {
    this->orbitPropagationAlgorithm = std::move(orbitPropagationAlgorithm);
    this->pipeline_.Complete(*this->orbitPropagationAlgorithm);
}

// TODO: Implement the ExecutePipeline and OutputResults methods for OrbitPipelineExecutor when
// the Data Serialization branch is merged

void OrbitPipelineExecutor::ExecutePipeline() {
    // Results are stored within the pipeline, can also be accessed
    // via this function call
    this->pipeline_.Run(this->options_.positionData);
}

void OrbitPipelineExecutor::OutputResults() {
    // TODO: Write the output to the datafile
    [[maybe_unused]] LocationRecord &futurePosition = this->pipeline_.GetProduct()->back();
    LOG_INFO("Calculated Future Position: (" << futurePosition.position.x << ", "
                                             << futurePosition.position.y << ", "
                                             << futurePosition.position.z << ") m"
                                             << " at time "
                                             << futurePosition.timestamp << " s");
}

}  // namespace found
