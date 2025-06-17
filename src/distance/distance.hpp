#ifndef DISTANCE_H
#define DISTANCE_H

#include <utility>
#include <memory>

#include "common/style.hpp"
#include "common/pipeline.hpp"
#include "common/spatial/attitude-utils.hpp"
#include "common/spatial/camera.hpp"

namespace found {

/**
 * The DistanceDeterminationAlgorithm class houses the Distance Determination Algorithm. This 
 * algorithm calculates the distance from Earth based on the pixels of Earth's Edge found in the image.
 * 
 * @note This algorithm performs optimally when the given Points is in polar order, i.e.
 * if we define the centroid of the points as P, for any
 * three consecutive points A B and C, angle APB is less than
 * angle APC
 */
class DistanceDeterminationAlgorithm : public Stage<Points, PositionVector> {
 public:
    // Constructs this
    DistanceDeterminationAlgorithm() = default;
    // Destroys this
    virtual ~DistanceDeterminationAlgorithm() {}
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
    SphericalDistanceDeterminationAlgorithm(decimal radius, Camera &&cam) : cam_(cam), radius_(radius) {}
    ~SphericalDistanceDeterminationAlgorithm() {}

    /**
     * Obtains the position of the planet relative to the camera
     * 
     * @param p The points on the edge of Earth (in the image taken
     * by the camera given to this)
     * 
     * @return PositionVector The position vector of the Earth relative
     * to the camera
     * 
     * @pre p must refer to points taken by the camera that was passed to
     * this
     * 
     * @post If p.size() < 3, then the result is exactly the zero vector
     * */
    PositionVector Run(const Points &p) override;

 protected:
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
    PreciseDecimal getRadius(Vec3* spats, Vec3 center);

    /**
     * Returns the scaled distance from earth
     * 
     * @param r The normalized radius
     * @param c The distance to the center of the small circle
     * 
     * @return The distance from earth as a Scalar
    */
    PreciseDecimal getDistance(PreciseDecimal r, PreciseDecimal c);

    /**
     * cam_ field instance describes the camera settings used for the photo taken
    */
    Camera cam_;

    /**
     * radius_ field instance describes the defined radius of earth. Should be 6378.0 (km)
    */
    decimal radius_;
};

/**
 * The IterativeSphericalDistanceDeterminationAlgorithm is a variation of the
 * SphericalDistanceDeterminationAlgorithm algorithm in that it runs it repeatedly
 * to use all the points given to it.
 * 
 * It uses softmax activation to figure out the plausibility of each guess
 */
class IterativeSphericalDistanceDeterminationAlgorithm : public SphericalDistanceDeterminationAlgorithm {
 public:
    /**
     * Creates a IterativeSphericalDistanceDeterminationAlgorithm
     * 
     * @param radius The radius of Earth
     * @param cam The camera used to capture the picture of Earth
     * @param minimumIterations The minimum number of iterations to perform
     * @param distanceRatio The maximum distance ratio between the evaluated and reference
     * positions to be considered "the same" distance 
     * @param discriminatorRatio The maximum ratio between the evaluated and reference loss
     * to accept for data
     */
    IterativeSphericalDistanceDeterminationAlgorithm(decimal radius,
                                                     Camera &&cam,
                                                     size_t minimumIterations,
                                                     decimal distanceRatio,
                                                     decimal discriminatorRatio)
      : SphericalDistanceDeterminationAlgorithm(radius, std::forward<Camera>(cam)),
        minimumIterations_(minimumIterations),
        distanceRatio_(distanceRatio),
        discriminatorRatio_(discriminatorRatio) {}
    ~IterativeSphericalDistanceDeterminationAlgorithm() = default;

    /**
     * Obtains the position of the planet relative to the camera
     * 
     * @param p The points on the edge of Earth (in the image taken
     * by the camera given to this)
     * 
     * @return PositionVector The position vector of the Earth relative
     * to the camera
     * 
     * @pre p must refer to points taken by the camera that was passed to
     * this
     * 
     * @pre p is radially sorted, i.e.
     * if we define the centroid of the points as P, for any
     * three consecutive points A B and C, angle APB is less than
     * angle APC
     * 
     * @post If p.size() < 3, then the result is exactly the zero vector
     * 
     * @note If minimumIterations (from constructor) is less than the size of
     * p, then it will increase the number of iterations to cover all of p
     * */
    PositionVector Run(const Points &p) override;

 private:
    /**
     * Generates a loss on a position vector
     * 
     * @param position The vector to evaluate
     * @param targetDistanceSq The target distance squared of the position
     * @param targetRadiusSq The target "radius" squared
     * @param projectedPoints The projected points to evaluate against
     * @param size The size of the projected points
     * 
     * @return The loss of position
     * 
     * @pre targetRadiusSq is not the true radius, but rather the
     * distance obtained between the radius vector and a circle
     * point when projected onto the unit sphere
     */
    decimal GenerateLoss(PositionVector &position,
                         decimal targetDistanceSq,
                         decimal targetRadiusSq,
                         std::unique_ptr<Vec3[]> &projectedPoints,
                         size_t size);
    /// The minimum number of iterations to use
    size_t minimumIterations_;
    /// The maximum distance ratio to accept
    decimal distanceRatio_;
    /// The maximum loss ratio to accept
    decimal discriminatorRatio_;
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
