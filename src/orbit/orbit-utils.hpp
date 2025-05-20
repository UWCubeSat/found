#pragma once

#include <functional>
#include <tuple>
#include <vector>
#include "../spatial/attitude-utils.hpp"
#include "style/style.hpp"


namespace found {

/**
 * @brief Performs Runge-Kutta 4th order integration (RK4) for a system of ODEs.
 * 
 * @param t Initial time
 * @param y Initial position vector
 * @param f Function to compute derivative of position
 * @param g Function to compute derivative of velocity
 * @param dt Time step
 * @param n Number of integration steps
 * @param v Initial velocity vector
 * @return Tuple containing time steps, position vectors, and velocity vectors
 */
std::tuple<std::vector<double>, std::vector<Vec3>, std::vector<Vec3>>
rk4(double t, const Vec3& y,
    std::function<Vec3(double, const Vec3&, const Vec3&)> f,
    std::function<Vec3(double, const Vec3&, const Vec3&)> g,
    double dt, int n, const Vec3& v);

/**
 * @brief Converts a state vector (position and velocity) to classical orbital elements.
 * 
 * @param R Position vector
 * @param V Velocity vector
 * @param mu Gravitational parameter
 * @return OrbitalElements structure containing classical orbital parameters
 */
OrbitParams state_vector_to_elements(const Vec3& R, const Vec3& V, double mu);

/**
 * @brief Converts orbital elements to a state vector (position and velocity).
 * 
 * @param elements Vector containing orbital elements [h, e, RA, incl, w, TA]
 * @param r Output position vector
 * @param v Output velocity vector
 * @param mu Gravitational parameter (defaults to Earth's value 398600.4418 km³/s²)
 */
void elements_to_state_vector(const std::vector<double>& elements, Vec3& r, Vec3& v, double mu);

/**
 * @brief Computes the specific angular momentum given eccentricity, altitude, and gravitational parameters.
 * 
 * @param e Eccentricity
 * @param altitude Altitude above the Earth's surface
 * @param mu Gravitational parameter
 * @param radius_earth Radius of the Earth
 * @return Specific angular momentum magnitude
 */
double altitude_to_specific_angular_momentum(double e, double altitude, double mu, double radius_earth);

}  // namespace found
