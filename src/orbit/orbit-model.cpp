#include <utility>
#include "orbit/orbit-model.hpp"
#include "orbit/orbit-utils.hpp"

namespace found {

Vec3 OrbitModel::get_avg_velocity(const Vec3& v1, const Vec3& v2, double dt) const {
    return (v1 + v2) * (1/dt);
}

std::pair<Vec3, Vec3> OrbitModel::propagate_r_v(const Vec3& r0, const Vec3& v0, double dt, double t_total) const {
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

    return {r_vals.back(), v_vals.back()};
}

OrbitalElements OrbitModel::get_orbital_elements(const Vec3& r, const Vec3& v) const {
    return state_vector_to_elements(r, v, mu_earth_km3s2);
}

Vec3 OrbitModel::first_deriv(double /*t*/, const Vec3& /*y*/, const Vec3& v) const {
    return v;
}

Vec3 OrbitModel::second_deriv(double /*t*/, const Vec3& y, const Vec3& /*v*/) const {
    double r = y.Magnitude();
    return y * (-mu_earth_km3s2 / (r * r * r));
}

}  // namespace found
