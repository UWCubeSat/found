#pragma once

#include "../spatial/attitude-utils.hpp"
#include <vector>

namespace found::model {

class OrbitModel {
public:
    Vec3 get_avg_velocity(const Vec3& v1, const Vec3& v2, double dt) const;

    std::pair<Vec3, Vec3> propagate_r_v(const Vec3& r0, const Vec3& v0, double dt, double t_total) const;

    std::vector<double> get_orbital_elements(const Vec3& r, const Vec3& v) const;

private:
    constexpr static double radius_earth_km = 6378.0;
    constexpr static double mu_earth_km3s2 = 398600.4418;

    Vec3 first_deriv(double t, const Vec3& y, const Vec3& v, double mu) const;

    Vec3 second_deriv(double t, const Vec3& y, const Vec3& v, double mu) const;
};

}  // namespace found::model
