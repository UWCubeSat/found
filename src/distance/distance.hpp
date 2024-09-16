#ifndef DISTANCE_H
#define DISTANCE_H

#include "style/style.hpp"

namespace found {

/**
 * The DistanceDeterminationAlgorithm class houses the Distance Determination Algorithm. This 
 * algorithm calculates the distance from Earth based on the pixels of Earth's Edge found in the image.
*/
class DistanceDeterminationAlgorithm {
 public:
    // Constructs this
    DistanceDeterminationAlgorithm() = default;
    // Destroys this
    virtual ~DistanceDeterminationAlgorithm();

    /**
     * Computes the distance of the satellite from Earth based on an image of Earth
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

/**
 * The DistanceDeterminationAlgorithm class houses the Distance Determination Algorithm. This 
 * algorithm calculates the distance from Earth based on the pixels of Earth's Edge found in the image.
 * 
 * @note This class assumes that Earth is a perfect sphere
*/
class SphericalDistanceDeterminationAlgorithm : public DistanceDeterminationAlgorithm {
 public:
    explicit SphericalDistanceDeterminationAlgorithm(float radius);
    ~SphericalDistanceDeterminationAlgorithm();

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    distFromEarth Run(char* image, Points &p/*More go here*/) override;
 private:
    // Fields specific to this algorithm, and helper methods
};

/**
 * The DistanceDeterminationAlgorithm class houses the Distance Determination Algorithm. This 
 * algorithm calculates the distance from Earth based on the pixels of Earth's Edge found in the image.
 * 
 * @note This class assumes that Earth is a perfect ellipse
*/
class EllipticDistanceDeterminationAlgorithm : public DistanceDeterminationAlgorithm {
 public:
    explicit EllipticDistanceDeterminationAlgorithm(distFromEarth radius);
    ~EllipticDistanceDeterminationAlgorithm();

    /**
    * Place documentation here. Press enter to automatically make a new line
    * */
    distFromEarth Run(char* image, Points &p/*More go here*/) override;
 private:
    // Fields specific to this algorithm, and helper methods
};

}  // namespace found

#endif
