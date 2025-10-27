#include <iostream>
#include <cmath>
#include <array>

const double MU = 3.986e14;

std::array<double, 3> cross_product(const std::array<double, 3>& a, const std::array<double, 3>& b) {
    return {a[1] * b[2] - a[2] * b[1],
            a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0]};
}

double norm(const std::array<double, 3>& vec) {
    return std::sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
}

std::array<double, 3> angular_momentum(const std::array<double, 3>& position, const std::array<double, 3>& velocity) {
    return cross_product(position, velocity);
}

std::array<double, 3> eccentricity_vector(const std::array<double, 3>& position,
                                          const std::array<double, 3>& velocity) {
    auto h = angular_momentum(position, velocity);
    double r_norm = norm(position);
    double v_norm = norm(velocity);

    std::array<double, 3> term1;
    for (int i = 0; i < 3; ++i)
        term1[i] = (v_norm * v_norm - MU / r_norm) * position[i] - (position[i] * velocity[i]) * velocity[i];

    std::array<double, 3> cross_vh = cross_product(velocity, h);
    std::array<double, 3> e_vec;
    for (int i = 0; i < 3; ++i)
        e_vec[i] = (1.0 / MU) * (cross_vh[i] - (MU / r_norm) * position[i]);

    return e_vec;
}

double eccentricity(const std::array<double, 3>& position, const std::array<double, 3>& velocity) {
    auto e_vec = eccentricity_vector(position, velocity);
    return norm(e_vec);
}


double semi_major_axis(const std::array<double, 3>& position, const std::array<double, 3>& velocity) {
    double r_norm = norm(position);
    double v_norm = norm(velocity);
    double energy = (v_norm * v_norm) / 2 - MU / r_norm;
    return -MU / (2 * energy);
}

double apoapsis(const std::array<double, 3>& position, const std::array<double, 3>& velocity) {
    double a = semi_major_axis(position, velocity);
    double e = eccentricity(position, velocity);
    return a * (1 + e);
}

double periapsis(const std::array<double, 3>& position, const std::array<double, 3>& velocity) {
    double a = semi_major_axis(position, velocity);
    double e = eccentricity(position, velocity);
    return a * (1 - e);
}

double inclination(const std::array<double, 3>& position, const std::array<double, 3>& velocity) {
    auto h = angular_momentum(position, velocity);
    return std::acos(h[2] / norm(h));
}

double raan(const std::array<double, 3>& position, const std::array<double, 3>& velocity) {
    auto h = angular_momentum(position, velocity);
    std::array<double, 3> K = {0, 0, 1};
    auto N_vec = cross_product(K, h);
    double N = norm(N_vec);
    return 2 * M_PI - std::acos(N_vec[0] / N);
}

double argument_of_periapsis(const std::array<double, 3>& position, const std::array<double, 3>& velocity) {
    auto h = angular_momentum(position, velocity);
    std::array<double, 3> K = {0, 0, 1};
    auto N_vec = cross_product(K, h);
    auto e_vec = eccentricity_vector(position, velocity);
    double e = norm(e_vec);
    double N = norm(N_vec);
    double e = eccentricity(position, velocity);
    return 2 * M_PI - std::acos((N_vec[0] * e_vec[0] + N_vec[1] * e_vec[1] + N_vec[2] * e_vec[2]) / (N * e));
}

double true_anomaly(const std::array<double, 3>& position, const std::array<double, 3>& velocity) {
    double r = norm(position);
    auto e_vec = eccentricity_vector(position, velocity);
    double e = norm(e_vec);
    double e = eccentricity(position, velocity);
    return std::acos((position[0] * e_vec[0] + position[1] * e_vec[1] + position[2] * e_vec[2]) / (r * e));
}