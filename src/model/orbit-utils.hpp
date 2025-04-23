#pragma once

#include "../spatial/attitude-utils.hpp"
#include <vector>
#include <functional>
#include <tuple>

namespace found::model {

std::tuple<std::vector<double>, std::vector<Vec3>, std::vector<Vec3>>
rk4(double t0,
    const Vec3& y0,
    std::function<Vec3(double, const Vec3&, const Vec3&, double)> f,
    std::function<Vec3(double, const Vec3&, const Vec3&, double)> g,
    double dt,
    int n,
    const Vec3& v0,
    double mu);

std::vector<double> state_vector_to_elements(const Vec3& r, const Vec3& v, double mu);

void elements_to_state_vector(const std::vector<double>& elements, Vec3& r, Vec3& v, double mu);

double altitude_to_specific_angular_momentum(double altitude, double mu);

}  // namespace found::model
