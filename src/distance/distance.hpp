#ifndef DISTANCE_H
#define DISTANCE_H

#include "style/style.hpp"
#include "pipeline/pipeline.hpp"
#include "spatial/attitude-utils.hpp"
#include "spatial/camera.hpp"

namespace found {

/**
 * The DistanceDeterminationAlgorithm class houses the Distance Determination Algorithm. This 
 * algorithm calculates the distance from Earth based on the pixels of Earth's Edge found in the image.
*/
class DistanceDeterminationAlgorithm : public Stage<Points, PositionVector> {
 public:
    // Constructs this
    DistanceDeterminationAlgorithm() = default;
    // Destroys this
    virtual ~DistanceDeterminationAlgorithm() = default;
};

/**
 * The DistanceDeterminationAlgorithm class houses the Distance Determination Algorithm. This 
 * algorithm calculates the distance from Earth based on the pixels of Earth's Edge found in the image.
 * 
 * @note This class assumes that Earth is a perfect sphere
*/
class SphericalDistanceDeterminationAlgorithm : public DistanceDeterminationAlgorithm {
 public:
    /**
     * Creates a SphericalDeterminationAlgorithm, which deduces
     * the Position vector of a sattelite from Earth by modeling
     * Earth as a sphere
     * 
     * @param radius The radius of Earth
     * @param cam The camera used to capture the picture of Earth
     */
    SphericalDistanceDeterminationAlgorithm(float radius, Camera &cam) : cam_(cam), radius_(radius) {}
    ~SphericalDistanceDeterminationAlgorithm() {}

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    PositionVector Run(const Points &p) override;

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
    /**
     * Initializes an EllipticDistanceDeterminationAlgorithm
     * 
     * @param radius The distance from Earth to use
     */
    explicit EllipticDistanceDeterminationAlgorithm(PositionVector radius);
    ~EllipticDistanceDeterminationAlgorithm();

    /**
    * Place documentation here. Press enter to automatically make a new line
    * */
    PositionVector Run(const Points &p) override;
 private:
    // Fields specific to this algorithm, and helper methods
};

}  // namespace found

#endif  // DISTANCE_H
