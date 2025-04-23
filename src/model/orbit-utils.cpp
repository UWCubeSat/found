#include "orbit-utils.hpp"
#include <cmath>
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
        double mu)
    {
        std::vector<Vec3> y_vals = {y0};
        std::vector<Vec3> v_vals = {v0};
        std::vector<double> t_vals = {t0};
    
        for (int i = 0; i < n; ++i) {
            double curr_t = t_vals.back();
            const Vec3& curr_y = y_vals.back();
            const Vec3& curr_v = v_vals.back();
    
            Vec3 k1 = f(curr_t, curr_y, curr_v, mu);
            Vec3 l1 = g(curr_t, curr_y, curr_v, mu);
    
            Vec3 k2 = f(curr_t + dt / 2.0, curr_y + k1 * (dt / 2.0), curr_v + l1 * (dt / 2.0), mu);
            Vec3 l2 = g(curr_t + dt / 2.0, curr_y + k1 * (dt / 2.0), curr_v + l1 * (dt / 2.0), mu);
    
            Vec3 k3 = f(curr_t + dt / 2.0, curr_y + k2 * (dt / 2.0), curr_v + l2 * (dt / 2.0), mu);
            Vec3 l3 = g(curr_t + dt / 2.0, curr_y + k2 * (dt / 2.0), curr_v + l2 * (dt / 2.0), mu);
    
            Vec3 k4 = f(curr_t + dt, curr_y + k3 * dt, curr_v + l3 * dt, mu);
            Vec3 l4 = g(curr_t + dt, curr_y + k3 * dt, curr_v + l3 * dt, mu);
    
            Vec3 y_new = curr_y + (k1 + k2 * 2.0 + k3 * 2.0 + k4) * (dt / 6.0);
            Vec3 v_new = curr_v + (l1 + l2 * 2.0 + l3 * 2.0 + l4) * (dt / 6.0);
            double t_new = curr_t + dt;
    
            y_vals.push_back(y_new);
            v_vals.push_back(v_new);
            t_vals.push_back(t_new);
        }
    
        return {t_vals, y_vals, v_vals};
    }

std::vector<double> state_vector_to_elements(const Vec3& r, const Vec3& v, double mu) {
    // Placeholder; normally computes a, e, i, Ω, ω, ν etc.
    std::vector<double> elements(6, 0.0); // Stub
    return elements;
}

void elements_to_state_vector(const std::vector<double>& elements, Vec3& r, Vec3& v, double mu) {
    // Placeholder
    r = Vec3(0, 0, 0);
    v = Vec3(0, 0, 0);
}

double altitude_to_specific_angular_momentum(double altitude, double mu) {
    double radius = 6378.0 + altitude;
    return std::sqrt(mu * radius);
}

}  // namespace found::model
