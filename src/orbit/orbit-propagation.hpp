#pragma once

#include <vector>
#include <utility>
#include "../spatial/attitude-utils.hpp"
#include "orbit/orbit-utils.hpp"
#include "location/location-record.hpp"
#include "pipeline/pipeline.hpp"
#include "style/style.hpp"

namespace found {

  class OrbitPropagationAlgorithm : public Stage<LocationRecord, std::vector<std::pair<Vec3, double>>> {

  private:
    /// The gravitational parameter (defaullt is for Earth in km^3/s^2)
    double mu_earth_km3s2_;
    /// The radius of the Earth in km
    double radius_earth_km_;
    /// The total time for propagation in seconds
    double total_time_;
    /// The time step for integration in seconds
    double dt_;

    public:

    /**
     * @brief Runs the orbit propagation algorithm on a given location record.
     *
     * This method takes an initial LocationRecord (containing position, velocity, and timestamp)
     * and propagates the state forward in time using the configured integration time step and 
     * gravitational parameter. It returns a vector of propagated positions with associated timestamps.
     *
     * @param record The input location record containing initial state information.
     * @return A vector of (position, time) pairs representing the propagated orbit.
     */
    std::vector<std::pair<Vec3, double>> Run(const LocationRecord& record) override;


    /**
     * @brief Constructs this OrbitPropagationAlgorithm.
     * 
     * @param mu The gravitational parameter (default is for Earth in km^3/s^2)
     * @param dt The time step for integration in seconds
     */
    explicit OrbitPropagationAlgorithm(double radius_earth_km = 6378.0, double mu_earth_km3s2 = 398600.4418, 
                                    double total_time, double dt);


    // Destroys this
    virtual ~OrbitPropagationAlgorithm();

    /**
     * @brief Computes the average velocity between two vectors over a time interval.
     *
     * @param v1 First velocity vector
     * @param v2 Second velocity vector
     * @param dt Time interval
     * @return Average velocity vector
     */
    Vec3 get_avg_velocity(const Vec3& v1, const Vec3& v2, double dt) const;

    /**
     * @brief Propagates the position and velocity vectors over a time interval.
     *
     * @param r0 Initial position vector
     * @param v0 Initial velocity vector
     * @param dt Time step for integration
     * @param t_total Total time to propagate
     * @return List of propagated positions and timestamps
     */
    std::pair<std::vector<Vec3>, std::vector<Vec3>> propagate_r_v(const Vec3& r0, const Vec3& v0, double dt, double t_total) const;

  };

   /**
   * @brief Computes the first derivative of the state vector (velocity).
   *
   * @param t Current time
   * @param y Position vector
   * @param v Velocity vector
   * @return First derivative of the state vector (velocity)
   */
  Vec3 first_deriv(double t, const Vec3& y, const Vec3& v) const;

  /**
   * @brief Computes the second derivative of the state vector (acceleration).
   *
   * @param t Current time
   * @param y Position vector
   * @param v Velocity vector
   * @return Second derivative of the state vector (acceleration)
   */
  Vec3 second_deriv(double t, const Vec3& y, const Vec3& v) const;

  } // namespace found