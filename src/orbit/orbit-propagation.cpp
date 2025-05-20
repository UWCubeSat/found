#include <utility>
#include "orbit/orbit-propagation.hpp"
#include "orbit/orbit-utils.hpp"
#include <stdexcept>  // for exceptions if needed
#include <cmath>      // for sqrt, pow

namespace found {

    OrbitPropagationAlgorithm::OrbitPropagationAlgorithm(double radius_earth_km, double mu_earth_km3s2, 
                                            double total_time, double dt)
    : mu_earth_km3s2_(mu_earth_km3s2),
      radius_earth_km_(radius_earth_km),
      total_time_(total_time),
      dt_(dt) {
        // if (radius_earth_km <= 0) {
        //     throw std::invalid_argument("Radius of Earth must be positive.");
        // }
        // if (mu_earth_km3s2 <= 0) {
        //     throw std::invalid_argument("Gravitational parameter must be positive.");
        // }
        // if (total_time <= 0) {
        //     throw std::invalid_argument("Total time must be positive.");
        // }
        // if (dt <= 0) {
        //     throw std::invalid_argument("Time step must be positive.");
        // }
    }

    OrbitPropagationAlgorithm::~OrbitPropagationAlgorithm() = default;

    std::vector<std::pair<Vec3, double>> OrbitPropagationAlgorithm::Run(const LocationRecord& record) {
        return propagate_r_v(record.position, record.velocity, dt_, total_time_);
      }
      

    Vec3 OrbitPropagationAlgorithm::get_avg_velocity(const Vec3& v1, const Vec3& v2, double dt) const {
    if (dt == 0) throw std::invalid_argument("Time interval dt cannot be zero.");
    return (v1 + v2) / dt;
    }

    std::pair<std::vector<Vec3>, std::vector<Vec3>> OrbitPropagationAlgorithm::propagate_r_v(
        const Vec3& r0, const Vec3& v0, double dt, double t_total) const {
    
        int n = static_cast<int>(t_total / dt);
        double t0 = 0;

        auto [t_vals, r_vals, v_vals] = rk4(t0, r0,
            [this](double t, const Vec3& y, const Vec3& v) {
                return this->first_deriv(t, y, v);
            },
            [this](double t, const Vec3& y, const Vec3& v) {
                return this->second_deriv(t, y, v);
            },
            dt, n, v0);

        return {r_vals, v_vals};
    }

    Vec3 OrbitModel::first_deriv(double /*t*/, const Vec3& /*y*/, const Vec3& v) const {
        return v;
    }

    Vec3 OrbitModel::second_deriv(double /*t*/, const Vec3& y, const Vec3& /*v*/) const {
        double r = y.Magnitude();
        return y * (-mu_earth_km3s2 / (r * r * r));
    }

}  // namespace found