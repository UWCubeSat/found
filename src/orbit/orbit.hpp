#ifndef ORBIT_H
#define ORBIT_H

#include <vector>

#include "common/spatial/attitude-utils.hpp"
#include "common/style.hpp"
#include "common/pipeline.hpp"

namespace found {

/**
 * An OrbitDeterminationAlgorithm is an object that houses the Orbit Projection Algorithm. This
 * algorithm finds the orbit path of the satellite from known position vectors relative to Earth
 * 
*/
class OrbitDeterminationAlgorithm : public Stage<std::vector<Vec3>, OrbitParams> {
 public:
    // Destroys this
    virtual ~OrbitDeterminationAlgorithm();
};

/**
 * An EllipticalOrbitDerminationAlgorithm is an object that houses the Orbit Projection Algorithm. 
 * This algorithm finds the orbit path of the satellite from known position vectors relative to Earth
 * by assuming that the orbital path is an ellipse that is spatially fixed.
 * 
*/
class EllipticalOrbitDerminationAlgorithm : public OrbitDeterminationAlgorithm {
 public:
    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    EllipticalOrbitDerminationAlgorithm(/*Params to initialze fields for this object*/);

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    ~EllipticalOrbitDerminationAlgorithm();

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    OrbitParams Run(const std::vector<Vec3> &positions /*Params to override the base class one*/) override;
 private:
    // Fields specific to this algorithm go here, and helper methods
};

/**
 * An PrecessionOrbitDeterminationAlgorithm is an object that houses the Orbit Projection Algorithm. 
 * This algorithm finds the orbit path of the satellite from known position vectors relative to Earth
 * by assuming that the orbital path is a Ellipse that is geometrically constant and rotates as well.
 * 
*/
class PrecessionOrbitDeterminationAlgorithm : public OrbitDeterminationAlgorithm {
 public:
    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    PrecessionOrbitDeterminationAlgorithm(/*Params to initialze fields for this object*/);

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    ~PrecessionOrbitDeterminationAlgorithm();

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    OrbitParams Run(const std::vector<Vec3> &positions /*Params to override the base class one*/) override;
 private:
    // Fields specific to this algorithm go here, and helper methods
};

}  // namespace found

#endif
