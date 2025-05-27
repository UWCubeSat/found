#ifndef ORBIT_H
#define ORBIT_H

#include <vector>
#include <utility>

#include "common/spatial/attitude-utils.hpp"
#include "common/pipeline.hpp"
#include "common/style.hpp"

namespace found {

// TODO: Remove once merged with Data Serialization branch
/**
 * @brief Represents a single location record containing a position and a timestamp.
 */
struct LocationRecord {
    /**
     * @brief The 3D position vector associated with this location.
     */
    Vec3 position;

    /**
     * @brief The timestamp corresponding to this location (in microseconds or appropriate unit).
     */
    uint64_t timestamp;
};

/**
 * OrbitPropagationAlgorithm is a stage that propagates an orbit over a specified time period.
 * It integrates the equations of motion to predict the satellite's trajectory using Runge-Kutta
 * 4 (RK4).
 */
class OrbitPropagationAlgorithm : public Stage<LocationRecord, std::vector<LocationRecord>> {
 public:
    /**
    * @brief Constructs this OrbitPropagationAlgorithm.
    * 
    * @param totalTime The total time to predict over
    * @param dt The time step for integration (seconds)
    * @param radius The radius of the celestial body (default is for Earth, in km)
    * @param mu The gravitational parameter (default is for Earth in km^3/s^2)
    */
    explicit OrbitPropagationAlgorithm(decimal totalTime, decimal dt, decimal radius, decimal mu);

    /// Destroys this
    virtual ~OrbitPropagationAlgorithm();
};

}  // namespace found

#endif  // ORBIT_H
