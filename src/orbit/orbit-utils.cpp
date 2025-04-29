#include "orbit/orbit-utils.hpp"
#include <cmath>
#include <tuple>
#include <vector>

namespace found {

std::tuple<std::vector<double>, std::vector<Vec3>, std::vector<Vec3>>
rk4(double t, const Vec3& y,
    std::function<Vec3(double, const Vec3&, const Vec3&)> f,
    std::function<Vec3(double, const Vec3&, const Vec3&)> g,
    double dt, int n, const Vec3& v) {
    std::vector<Vec3> y_vals = { y };
    std::vector<Vec3> v_vals = { v };
    std::vector<double> t_vals = { t };

    for (int i = 0; i < n; ++i) {
        double curr_t = t_vals.back();
        Vec3 curr_y = y_vals.back();
        Vec3 curr_v = v_vals.back();

        Vec3 k1 = f(curr_t, curr_y, curr_v);
        Vec3 l1 = g(curr_t, curr_y, curr_v);

        Vec3 k2 = f(curr_t + dt / 2.0, curr_y + k1 * (dt / 2.0), curr_v + l1 * (dt / 2.0));
        Vec3 l2 = g(curr_t + dt / 2.0, curr_y + k1 * (dt / 2.0), curr_v + l1 * (dt / 2.0));

        Vec3 k3 = f(curr_t + dt / 2.0, curr_y + k2 * (dt / 2.0), curr_v + l2 * (dt / 2.0));
        Vec3 l3 = g(curr_t + dt / 2.0, curr_y + k2 * (dt / 2.0), curr_v + l2 * (dt / 2.0));

        Vec3 k4 = f(curr_t + dt, curr_y + k3 * dt, curr_v + l3 * dt);
        Vec3 l4 = g(curr_t + dt, curr_y + k3 * dt, curr_v + l3 * dt);

        Vec3 y_new = curr_y + (k1 + k2 * 2.0 + k3 * 2.0 + k4) * (dt / 6.0);
        Vec3 v_new = curr_v + (l1 + l2 * 2.0 + l3 * 2.0 + l4) * (dt / 6.0);
        double t_new = curr_t + dt;

        y_vals.push_back(y_new);
        v_vals.push_back(v_new);
        t_vals.push_back(t_new);
    }

    return { t_vals, y_vals, v_vals };
}

OrbitalElements state_vector_to_elements(const Vec3& R, const Vec3& V, double mu) {
    const double eps = 1e-10;

    double r = R.Magnitude();
    // double v = V.Magnitude();
    double vr = (R*V) / r;

    Vec3 H = R.CrossProduct(V);
    double h = H.Magnitude();

    double incl = std::acos(H.z / h);

    Vec3 K(0.0, 0.0, 1.0);
    Vec3 N = K.CrossProduct(H);
    double n = N.Magnitude();

    double RA = 0.0;
    if (n > eps) {
        RA = std::acos(N.x / n);
        if (N.y < 0) {
            RA = 2.0 * M_PI - RA;
        }
    }

    Vec3 E_vec = (R * ((V * V) - (mu / r)) - V * (r * vr)) * (1/mu);  // Double check this line
    double e = E_vec.Magnitude();

    double w = 0.0;
    if (n > eps && e > eps) {
        w = std::acos((N*E_vec) / (n * e));
        if (E_vec.z < 0) {
            w = 2.0 * M_PI - w;
        }
    }

    double TA = 0.0;
    if (e > eps) {
        TA = std::acos((E_vec*R) / (e * r));
        if (vr < 0) {
            TA = 2.0 * M_PI - TA;
        }
    } else {
        // Circular orbit case
        double cos_TA = R.x / r;
        if (R.y >= 0) {
            TA = std::acos(cos_TA);
        } else {
            TA = 2.0 * M_PI - std::acos(cos_TA);
        }
    }

    return { h, e, RA, incl, w, TA };
}

void elements_to_state_vector(const OrbitalElements& elements, Vec3& r, Vec3& v, double mu) {
    double h = elements.h;
    double e = elements.e;
    double RA = elements.RA;
    double incl = elements.incl;
    double w = elements.w;
    double TA = elements.TA;

    // Position and velocity in perifocal coordinates
    double cos_TA = std::cos(TA);
    double sin_TA = std::sin(TA);
    double r_perifocal_x = (h * h / mu) / (1 + e * cos_TA) * cos_TA;
    double r_perifocal_y = (h * h / mu) / (1 + e * cos_TA) * sin_TA;
    double r_perifocal_z = 0.0;

    double v_perifocal_x = -(mu / h) * sin_TA;
    double v_perifocal_y = (mu / h) * (e + cos_TA);
    double v_perifocal_z = 0.0;

    // Rotation matrices (Z-X-Z rotation)
    double cos_RA = std::cos(RA);
    double sin_RA = std::sin(RA);
    double cos_incl = std::cos(incl);
    double sin_incl = std::sin(incl);
    double cos_w = std::cos(w);
    double sin_w = std::sin(w);

    // Combined rotation matrix from perifocal to geocentric equatorial frame
    double R11 = cos_RA * cos_w - sin_RA * sin_w * cos_incl;
    double R12 = cos_RA * sin_w + sin_RA * cos_w * cos_incl;
    double R13 = sin_RA * sin_incl;

    double R21 = -sin_RA * cos_w - cos_RA * sin_w * cos_incl;
    double R22 = -sin_RA * sin_w + cos_RA * cos_w * cos_incl;
    double R23 = cos_RA * sin_incl;

    double R31 = sin_w * sin_incl;
    double R32 = -cos_w * sin_incl;
    double R33 = cos_incl;

    // Rotate position
    r.x = R11 * r_perifocal_x + R12 * r_perifocal_y + R13 * r_perifocal_z;
    r.y = R21 * r_perifocal_x + R22 * r_perifocal_y + R23 * r_perifocal_z;
    r.z = R31 * r_perifocal_x + R32 * r_perifocal_y + R33 * r_perifocal_z;

    // Rotate velocity
    v.x = R11 * v_perifocal_x + R12 * v_perifocal_y + R13 * v_perifocal_z;
    v.y = R21 * v_perifocal_x + R22 * v_perifocal_y + R23 * v_perifocal_z;
    v.z = R31 * v_perifocal_x + R32 * v_perifocal_y + R33 * v_perifocal_z;
}

double altitude_to_specific_angular_momentum(double e, double altitude, double mu, double radius_earth) {
    double rp = radius_earth + altitude;
    double a = rp / (1.0 - e);
    double h = std::sqrt(mu * a * (1.0 - e * e));
    return h;
}

}  // namespace found
