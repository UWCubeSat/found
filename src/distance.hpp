#ifndef DISTANCE_H
#define DISTANC_H

#include "edge.hpp"


class DistanceDeterminationAlgorithm {
public:
    DistanceDeterminationAlgorithm(float radius /*More params go here*/) : earthRadius(radius) {};
    
    // Main algorithm for distance determination. Returns distance from Earth
    virtual float Go(char* image, Points &p /*More go here*/);
private:
    //Fields common to all algorithms of this type
    float earthRadius;
};


class SphericalDistanceDeterminationAlgorithm : public DistanceDeterminationAlgorithm {
public:
    SphericalDistanceDeterminationAlgorithm(float radius) : DistanceDeterminationAlgorithm(radius) {};
    
    // Main algorithm for distance determination. Returns distance from Earth. MUST OVERRIDE
    float Go(char* image, Points &p/*More go here*/) override;
private:
    //Fields specific to this algorithm
};


class EllipticDistanceDeterminationAlgorithm : public DistanceDeterminationAlgorithm {
public:
    EllipticDistanceDeterminationAlgorithm(float radius) : DistanceDeterminationAlgorithm(radius) {};

    // Main algorithm for distance determination. MUST OVERRIDE
    float Go(char* image, Points &p/*More go here*/) override;
private:
    //Fields specific to this algorithm
};


#endif