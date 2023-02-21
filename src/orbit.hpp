#ifndef ORBIT_H
#define ORBIT_H

#include <vector>

#include "attitude-utils.hpp"
#include "style.hpp"

namespace found {

// Always name whatever your return type is this type
typedef std::vector<decimal> OrbitParams;

class OrbitDeterminationAlgorithm {
public:
    OrbitDeterminationAlgorithm();
    virtual ~OrbitDeterminationAlgorithm();
    virtual OrbitParams Run(/*Params common to this type*/) = 0;
private:

};


class EllipticalOrbitDerminationAlgorithm : public OrbitDeterminationAlgorithm {
public:
    EllipticalOrbitDerminationAlgorithm(/*Params to initialze fields for this object*/);
    ~EllipticalOrbitDerminationAlgorithm();
    OrbitParams Run(/*Params to override the base class one*/) override;
private:
    // Fields specific to this algorithm go here
};


class PrecessionOrbitDeterminationAlgorithm : public OrbitDeterminationAlgorithm {
public:
    PrecessionOrbitDeterminationAlgorithm(/*Params to initialze fields for this object*/);
    ~PrecessionOrbitDeterminationAlgorithm();
    OrbitParams Run(/*Params to override the base class one*/) override;
private:
    // Fields specific to this algorithm go here 
};

}

#endif