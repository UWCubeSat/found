#pragma once

#include <vector>
#include <utility>
#include "../spatial/attitude-utils.hpp"
#include "orbit/orbit-utils.hpp"

namespace found {

class OrbitModel {
 public:
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
   * @return State vector (position, velocity) after propagation
   */
  std::pair<Vec3, Vec3> propagate_r_v(const Vec3& r0, const Vec3& v0, double dt, double t_total) const;

  /**
   * @brief Computes the orbital elements from a state vector (position, velocity).
   *
   * @param r Position vector
   * @param v Velocity vector
   * @return Set of orbital elements [h, e, RA, incl, w, TA]
   */
  OrbitalElements get_orbital_elements(const Vec3& r, const Vec3& v) const;

 private:
  constexpr static double radius_earth_km = 6378.0;  ///< Mean radius of Earth in kilometers
  constexpr static double mu_earth_km3s2 = 398600.4418;  ///< Gravitational parameter for Earth in km³/s²

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
};

}  // namespace found
