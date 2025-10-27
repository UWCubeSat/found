#include <iostream>
#include <cmath>
#include <functional>
#include <tuple>

using namespace std;

double b_const = 1.0;
double x = 0;
double y = 1;
double dt = 0.01;
double v0 = abs(y-x)/dt;

double a(double t,double y) {
   return b_const*cos(t);
}

double a_prime(double t,double y) {
   return -1*b_const*sin(t);
}

double a_double_prime(double t,double y) {
   return -1*b_const*cos(t);
}

double b(double t, double y) {
   return b_const*sin(t);
}

double b_prime(double t,double y) {
   return b_const*cos(t);
}

double b_double_prime(double t,double y) {
   return -1*b_const*sin(t);
}

double f(double t,double y,double v) {
   return a_prime(t,y);
}

double g(double t,double y,double v) {
   return a_double_prime(t,y);
}

double f1(double t,double y,double v) {
   return b_prime(t,y);
}

double g1(double t,double y,double v) {
   return b_double_prime(t,y);
}

std::tuple<std::vector<double>, std::vector<double>, std::vector<double>> rungeKutta( double t, double y, double dt, double n, double v, std::function<double(double, double, double)> f, std::function<double(double, double, double)> g ){

   vector<double> y_vals = {y};
   vector<double> v_vals = {v};
   vector<double> t_vals = {t};;
   for(int i = 1; i <= n; i++) {

      double k1 = f(t, y_vals[i-1], v_vals[i-1]);
      double l1 = g(t, y_vals[i-1], v_vals[i-1]);
      double k2 = f(t + dt/2, y_vals[i-1] + dt * (k1 / 2), v_vals[i-1] + dt * (l1 / 2));
      double l2 = g(t + dt/2, y_vals[i-1] + dt * (k1 / 2), v_vals[i-1] + dt * (l1 / 2));
      double k3 = f(t + dt/2, y_vals[i-1] + dt * (k2 / 2), v_vals[i-1] + dt * (l2 / 2));
      double l3 = g(t + dt/2, y_vals[i-1] + dt * (k2 / 2), v_vals[i-1] + dt * (l2 / 2));
      double k4 = f(t + dt, y_vals[i-1] + dt * k3, v_vals[i-1] + dt * l3);
      double l4 = g(t + dt, y_vals[i-1] + dt * k3, v_vals[i-1] + dt * l3);
      double y_new = y_vals[i-1] + (1.0 / 6) * dt * (k1 + 2*k2 + 2*k3 + k4);
      y_vals.push_back(y_new);
      double v_new = v_vals[i-1] + (1.0 / 6) * dt * (l1 + 2*l2 + 2*l3 + l4);
      v_vals.push_back(v_new);
      t += dt;
      t_vals.push_back(t);

   }
   return std::make_tuple(t_vals, y_vals, v_vals);
}



int main() {

   auto [t_vals, y_vals, v_vals] = rungeKutta(0,1,dt,1000,v0,f,g);
   auto [t1_vals, x_vals, v1_vals] = rungeKutta(0,1,dt,1000,v0,f1,g1);
   auto [t2_vals, z_vals, v2_vals] = rungeKutta(0,1,dt,1000,v0,f,g);

   std::cout << "\n\nX values:\n";
   for(double val : x_vals) {
      std::cout << val << " ";
   }
   std::cout << "\n\nV1 values:\n";
   for(double val : v1_vals) {
      std::cout << val << " ";
   }
   std::cout << "\n\nZ values:\n";
   for(double val : z_vals) {
      std::cout << val << " ";
   }
   std::cout << std::endl;
   

   return 0;
}