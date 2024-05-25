#ifndef DISTANCE_H
#define DISTANCE_H

#include "style.hpp"
#include "attitude-utils.hpp"
#include "camera.hpp"

namespace found {

/**
 * The DistanceDeterminationAlgorithm class houses the Distance Determination Algorithm. This 
 * algorithm calculates the distance from Earth based on the pixels of Earth's Edge found in the image.
*/
class DistanceDeterminationAlgorithm {
public:

    // Destroys this
    virtual ~DistanceDeterminationAlgorithm() {};
    
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
    virtual distFromEarth Run(char* image, Points &p, int imageWidth, int imageHeight /*More go here*/) = 0;
};

/**
 * The DistanceDeterminationAlgorithm class houses the Distance Determination Algorithm. This 
 * algorithm calculates the distance from Earth based on the pixels of Earth's Edge found in the image.
 * 
 * @note This class assumes that Earth is a perfect sphere
*/
class SphericalDistanceDeterminationAlgorithm : public DistanceDeterminationAlgorithm {
public:
    SphericalDistanceDeterminationAlgorithm(float radius, Camera &cam) : cam_(cam), radius_(radius) {};
    ~SphericalDistanceDeterminationAlgorithm() {};
    
    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    distFromEarth Run(char* image, Points &p, int imageWidth, int imageHeight/*More go here*/) override;
private:
    // Fields specific to this algorithm, and helper methods
    /**
     *Returns the center of earth as a 3d Vector
     *
     * @param spats The normalized spatial coordinates used to find the center 
     * 
     * @return The center of earth as a 3d Vector
    */
    Vec3 getCenter(Vec3* spats);

    /**
     * Returns the radius of the calculated "earth" (normalized)
     * 
     * @param spats The normalized spatial coordinates
     * @param center The center of the earth as a 3d Vector
     * 
     * @return The radius of earth normalized
    */
    decimal getRadius(Vec3* spats, Vec3 center);

    /**
     * Returns the scaled distance from earth
     * 
     * @param r The normalized radius
     * 
     * @return The distance from earth as a Scalar
    */
    decimal getDistance(decimal r);

    /**
     * Solves the whole thing, calculating the final distance from the earth as a 3d Vector
     * 
     * @param pts The points on the image (Not used)
     * @param R The given radius (Currently not used, Radius is set to 6378.0)
     * 
     * @return The distance from earth as a 3d Vector
    */
    distFromEarth solve(Points& pts, int R);
    
    /**
     * cam_ field instance describes the camera settings used for the photo taken
    */
    Camera cam_;

    /**
     * radius_ field instance describes the defined radius of earth. Should be 6378.0 (km)
    */
    float radius_;

};

/**
 * The DistanceDeterminationAlgorithm class houses the Distance Determination Algorithm. This 
 * algorithm calculates the distance from Earth based on the pixels of Earth's Edge found in the image.
 * 
 * @note This class assumes that Earth is a perfect ellipse
*/
class EllipticDistanceDeterminationAlgorithm : public DistanceDeterminationAlgorithm {
public:
    EllipticDistanceDeterminationAlgorithm(distFromEarth radius);
    ~EllipticDistanceDeterminationAlgorithm();

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    distFromEarth Run(char* image, Points &p, int imageWidth, int imageHeight/*More go here*/) override;
private:
    //Fields specific to this algorithm, and helper methods
};

}


#endif