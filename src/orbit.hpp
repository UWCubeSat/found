#ifndef ORBIT_H
#define ORBIT_H

#include "attitude-utils.hpp"
#include "style.hpp"

namespace found {

/**
 * An OrbitDeterminationAlgorithm is an object that houses the Orbit Projection Algorithm. This
 * algorithm finds the orbit path of the satellite from known position vectors relative to Earth
 * 
*/
class OrbitDeterminationAlgorithm {
public:

    // Destroys this
    virtual ~OrbitDeterminationAlgorithm();

    /**
     * Finds orbit parameters based off of given positional vectors
     * 
     * @param positions A list describing known points along satellite path
     * 
     * @return An OrbitParams, which describes the characteristics of the orbital path
     * 
     * @note See style.hpp for more detailed documentation of OrbitParams
    */
    virtual OrbitParams Run(std::vector<Vec3> &positions /*Params common to this type*/) = 0;
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
    OrbitParams Run(std::vector<Vec3> &positions /*Params to override the base class one*/) override;
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
    OrbitParams Run(std::vector<Vec3> &positions /*Params to override the base class one*/) override;
private:
    // Fields specific to this algorithm go here, and helper methods
};

}

#endif