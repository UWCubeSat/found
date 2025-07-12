#ifndef SRC_ORBIT_ORBIT_HPP_
#define SRC_ORBIT_ORBIT_HPP_

#include <vector>
#include <utility>

#include "common/spatial/attitude-utils.hpp"
#include "common/pipeline/stages.hpp"
#include "common/style.hpp"

namespace found {

/**
 * The OrbitPropagationAlgorithm is an algorithm that propagates an orbit
 * over a specified time period.
 */
class OrbitPropagationAlgorithm : public FunctionStage<LocationRecords, LocationRecords> {
 public:
    /// Constructs this
    OrbitPropagationAlgorithm() = default;
    /// Destroys this
    virtual ~OrbitPropagationAlgorithm() {}
};

/**
 * OrbitPropagationAlgorithm is a stage that propagates an orbit over a specified time period.
 * It integrates the equations of motion to predict the satellite's trajectory using Runge-Kutta
 * 4 (RK4).
 */
class ApproximateOrbitPropagationAlgorithm : public OrbitPropagationAlgorithm {
 public:
    /**
    * @brief Constructs this OrbitPropagationAlgorithm.
    * 
    * @param totalTime The total time to predict over
    * @param dt The time step for integration (seconds)
    * @param radius The radius of the celestial body (default is for Earth, in km)
    * @param mu The gravitational parameter (default is for Earth in km^3/s^2)
    */
    explicit ApproximateOrbitPropagationAlgorithm(decimal totalTime, decimal dt, decimal radius, decimal mu)
        : totalTime_(totalTime), dt_(dt), radius_(radius), mu_(mu) {}

    /// Destroys this
    ~ApproximateOrbitPropagationAlgorithm();

    /**
     * Projects an orbit
     * 
     * @param data The past data to use
     * 
     * @return Predicted positions in the future
     */
    LocationRecords Run(const LocationRecords &data) override;

 private:
    /// The total time to predict over
    decimal totalTime_;
    /// The time step for integration (seconds)
    decimal dt_;
    /// The radius of the celestial body (default is for Earth, in km)
    decimal radius_;
    /// The gravitational parameter (default is for Earth in km^3/s^2)
    decimal mu_;
};

}  // namespace found

#endif  // SRC_ORBIT_ORBIT_HPP_
