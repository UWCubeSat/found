#include "orbit/orbital-elements.hpp"
#include <stdexcept>
#include <cmath>

namespace found {

constexpr double mu_earth_km3s2 = 398600.4418;  // Earth's gravitational parameter in km³/s²

OrbitalElementsCalculation::~OrbitalElementsCalculation() = default;

OrbitParams OrbitalElementsCalculation::get_orbital_elements(const Vec3& r, const Vec3& v) const {
    return state_vector_to_elements(r, v, mu_earth_km3s2);
}

}  // namespace found
