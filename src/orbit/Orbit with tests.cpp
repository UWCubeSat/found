#include <iostream>
#include <vector>
#include <cmath>

const double b_const = 1.0;

double a(double t, double y) {
    return b_const * cos(t);
}
double a_prime(double t, double y) {
    return -1.0 * b_const * sin(t);
}
double a_double_prime(double t, double y) {
    return -1.0 * b_const * cos(t);
}
double b_func(double t, double y) {
    return b_const * sin(t);
}
double b_prime(double t, double y) {
    return b_const * cos(t);
}
double b_double_prime(double t, double y) {
    return -b_const * sin(t);
}
double f(double t, double y, double v) {
    return a_prime(t, y);
}
double g(double t, double y, double v) {
    return a_double_prime(t, y);
}
double f1(double t, double y, double v) {
    return b_prime(t, y);
}
double g1(double t, double y, double v) {
    return b_double_prime(t, y);
}

void rungeKutta2_y(double t, double y, double (*f)(double, double, double), double (*g)(double, double, double), double dt, int n, double v,
                   std::vector<double> &t_vals, std::vector<double> &y_vals, std::vector<double> &v_vals) {
    t_vals.push_back(t);
    y_vals.push_back(y);
    v_vals.push_back(v);

    for (int i = 0; i < n; ++i) {
        double k1 = f(t, y_vals.back(), v_vals.back());
        double l1 = g(t, y_vals.back(), v_vals.back());
        double k2 = f(t + dt / 2, y_vals.back() + dt * (k1 / 2), v_vals.back() + dt * (l1 / 2));
        double l2 = g(t + dt / 2, y_vals.back() + dt * (k1 / 2), v_vals.back() + dt * (l1 / 2));
        double k3 = f(t + dt / 2, y_vals.back() + dt * (k2 / 2), v_vals.back() + dt * (l2 / 2));
        double l3 = g(t + dt / 2, y_vals.back() + dt * (k2 / 2), v_vals.back() + dt * (l2 / 2));
        double k4 = f(t + dt, y_vals.back() + dt * k3, v_vals.back() + dt * l3);
        double l4 = g(t + dt, y_vals.back() + dt * k3, v_vals.back() + dt * l3);

        double y_new = y_vals.back() + (1.0 / 6.0) * dt * (k1 + 2 * k2 + 2 * k3 + k4);
        double v_new = v_vals.back() + (1.0 / 6.0) * dt * (l1 + 2 * l2 + 2 * l3 + l4);

        t += dt;
        t_vals.push_back(t);
        y_vals.push_back(y_new);
        v_vals.push_back(v_new);
    }
}

void rungeKutta2_x(double t, double x, double (*f)(double, double, double), double (*g)(double, double, double), double dt, int n, double v,
                   std::vector<double> &t_vals, std::vector<double> &x_vals, std::vector<double> &v_vals) {
    t_vals.push_back(t);
    x_vals.push_back(x);
    v_vals.push_back(v);

    for (int i = 0; i < n; ++i) {
        double k1 = f(t, x_vals.back(), v_vals.back());
        double l1 = g(t, x_vals.back(), v_vals.back());
        double k2 = f(t + dt / 2, x_vals.back() + dt * (k1 / 2), v_vals.back() + dt * (l1 / 2));
        double l2 = g(t + dt / 2, x_vals.back() + dt * (k1 / 2), v_vals.back() + dt * (l1 / 2));
        double k3 = f(t + dt / 2, x_vals.back() + dt * (k2 / 2), v_vals.back() + dt * (l2 / 2));
        double l3 = g(t + dt / 2, x_vals.back() + dt * (k2 / 2), v_vals.back() + dt * (l2 / 2));
        double k4 = f(t + dt, x_vals.back() + dt * k3, v_vals.back() + dt * l3);
        double l4 = g(t + dt, x_vals.back() + dt * k3, v_vals.back() + dt * l3);

        double x_new = x_vals.back() + (1.0 / 6.0) * dt * (k1 + 2 * k2 + 2 * k3 + k4);
        double v_new = v_vals.back() + (1.0 / 6.0) * dt * (l1 + 2 * l2 + 2 * l3 + l4);

        t += dt;
        t_vals.push_back(t);
        x_vals.push_back(x_new);
        v_vals.push_back(v_new);
    }
}

int main() {
    double x = 0;
    double y = 1;
    double dt = 0.01;
    int n = 1000;
    double v0 = std::abs((y - x)) / dt;
    std::vector<double> t_vals, quad_y, v;
    rungeKutta2_y(0, 1, f, g, dt, n, v0, t_vals, quad_y, v);

    std::vector<double> t_vals2, quad_x, v2;
    rungeKutta2_x(0, 1, f1, g1, dt, n, v0, t_vals2, quad_x, v2);

    std::vector<double> t_vals3, quad_z, v3;
    rungeKutta2_y(0, 1, f, g, dt, n, v0, t_vals3, quad_z, v3);

    std::vector<double> quad_actual_values_y, quad_actual_values_x, quad_actual_values_z;
    for (int i = 0; i < t_vals.size(); ++i) {
        quad_actual_values_y.push_back(a(t_vals[i], quad_y[i]));
        quad_actual_values_z.push_back(a(t_vals[i], quad_z[i]));
    }
    for (int i = 0; i < t_vals2.size(); ++i) {
        quad_actual_values_x.push_back(b_func(t_vals2[i], quad_x[i]));
    }

    for (int i = 0; i < t_vals.size(); ++i) {
        std::cout << quad_x[i] << " " << quad_y[i] << " " << quad_z[i] << std::endl;
    }
    return 0;
}