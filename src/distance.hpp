#ifndef DISTANCE_H
#define DISTANCE_H

#include "edge.hpp"
#include "style.hpp"

namespace found {

/**
 * The DistanceDeterminationAlgorithm class houses the Distance Determination Algorithm. This 
 * algorithm calculates the distance from Earth based on the pixels of Earth's Edge found in the image.
*/
class DistanceDeterminationAlgorithm {
public:
    /**
     * Creates a Distance Determination Object
    */
    DistanceDeterminationAlgorithm();

    // Destroys this
    virtual ~DistanceDeterminationAlgorithm();
    
    /**
     * Runs the Distance Determination Algorithm, which finds the distance of the 
     * satellite from Earth based on the image of Earth
     * 
     * @param image The image of Earth, represented as a character array with values from 0-255
     * that represents the black/white color of each pixel
     * @param p The Points that we find on Earth's horizon with respect to the image coordinate
     * system
     * 
     * @return The distance of the satellite from Earth
    */
    virtual distFromEarth Run(char* image, Points &p /*More go here*/) = 0;
};


class SphericalDistanceDeterminationAlgorithm : public DistanceDeterminationAlgorithm {
public:
    SphericalDistanceDeterminationAlgorithm(float radius);
    ~SphericalDistanceDeterminationAlgorithm();
    
    // Main algorithm for distance determination. Returns distance from Earth. MUST OVERRIDE
    distFromEarth Run(char* image, Points &p/*More go here*/) override;
private:
    // Fields specific to this algorithm, and helper methods
};


class EllipticDistanceDeterminationAlgorithm : public DistanceDeterminationAlgorithm {
public:
    EllipticDistanceDeterminationAlgorithm(distFromEarth radius);
    ~EllipticDistanceDeterminationAlgorithm();

    // Main algorithm for distance determination. MUST OVERRIDE
    distFromEarth Run(char* image, Points &p/*More go here*/) override;
private:
    //Fields specific to this algorithm, and helper methods
};

}


#endif