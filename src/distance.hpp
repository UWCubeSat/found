#ifndef DISTANCE_H
#define DISTANCE_H

#include "edge.hpp"
#include "style.hpp"

namespace found {

// Always name your output distFromEarth
typedef decimal distFromEarth;

class DistanceDeterminationAlgorithm {
public:
    DistanceDeterminationAlgorithm();
    virtual ~DistanceDeterminationAlgorithm();
    
    // Main algorithm for distance determination. Returns distance from Earth
    virtual distFromEarth Run(char* image, Points &p /*More go here*/) = 0;
private:
    // Should be nothing, if all algos share a field, better to put it in their specific class
};


class SphericalDistanceDeterminationAlgorithm : public DistanceDeterminationAlgorithm {
public:
    SphericalDistanceDeterminationAlgorithm(float radius);
    ~SphericalDistanceDeterminationAlgorithm();
    
    // Main algorithm for distance determination. Returns distance from Earth. MUST OVERRIDE
    distFromEarth Run(char* image, Points &p/*More go here*/) override;
private:
    //Fields specific to this algorithm
};


class EllipticDistanceDeterminationAlgorithm : public DistanceDeterminationAlgorithm {
public:
    EllipticDistanceDeterminationAlgorithm(distFromEarth radius);
    ~EllipticDistanceDeterminationAlgorithm();

    // Main algorithm for distance determination. MUST OVERRIDE
    distFromEarth Run(char* image, Points &p/*More go here*/) override;
private:
    //Fields specific to this algorithm
};

}


#endif