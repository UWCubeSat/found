#pragma once

#include <vector>
#include <utility>
#include "../spatial/attitude-utils.hpp"
#include "orbit/orbit-utils.hpp"
#include "pipeline/pipeline.hpp"
#include "style/style.hpp"

namespace found {

  class OrbitalElementsCalculation : public Stage<std::vector<std::pair<Vec3, double>>, OrbitParams> {
    public:
    // Constructs this
    OrbitalElementsCalculation() = default;
    // Destroys this
    virtual ~OrbitalElementsCalculation();

    /**
     * @brief Computes the orbital elements from a state vector (position, velocity).
     *
     * @param r Position vector
     * @param v Velocity vector
     * @return Set of orbital elements [h, e, RA, incl, w, TA]
     */
    OrbitParams get_orbital_elements(const Vec3& r, const Vec3& v) const;

  };

  } // namespace found