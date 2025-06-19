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
 * It uses
 * - selective randomization of Points, using a Quadratic or Quartic
 *   base distribution to prioritize points farther from selected
 *   points within triplets
 * - loss criterion to evaluate each guess
 * - softmax activation to figure out the plausibility of each guess
 * 
 * @note Testing data on `test/common/assets/example_earth1.png`:
 * 
 * SDDA -> (1.0456e+07, -67903.8, -972.935) m
 * - Distance Error: 0.752384891562%
 * - Angle Error: 1339.6805912772 arcseconds
 * - Execution Time: < 1 sec
 * 
 * ISDDA(100000, 0.8, INF, Quadratic Radius Loss AND Randomization) -> (1.0384e+07, -12571.3, -1057.05) m
 * - Distance Error: 0.0565676042517%
 * - Angle Error: 250.59497104116 arcseconds
 * - Execution Time: 11 sec
 * 
 * ISDDA(100000, 0.8, INF, Quartic Radius Loss OR Randomization) -> (1.03781e+07, -11536.7, -927.331) m
 * - Distance Error: 0.000294332681557%
 * - Angle Error: 230.031583013 arcseconds
 * - Execution Time: 11 sec
 * 
 * In optimized mode (-O3), all algorithms are less than 1 second.
 */
class IterativeSphericalDistanceDeterminationAlgorithm : public SphericalDistanceDeterminationAlgorithm {
 public:
    /**
     * Creates a IterativeSphericalDistanceDeterminationAlgorithm
     * 
     * @param radius The radius of Earth
     * @param cam The camera used to capture the picture of Earth
     * @param minimumIterations The minimum number of iterations to perform
     * @param distanceRatio The maximum distance error between the evaluated and reference
     * positions to be considered "the same" distance 
     * @param discriminatorRatio The maximum ratio between the evaluated and reference loss
     * to accept for data
     * 
     * @note Setting distanceRatio to DECIMAL_INF will exclude distance loss from loss
     * calculations
     * 
     * @note Setting discriminatorRatio to DECIMAL_INF will include all generated points
     * in the final point
     * 
     * @note Additional hyperparameters are the L_RADIUS_MOD function and PDF macros,
     * defined within distance.cpp
     */
    IterativeSphericalDistanceDeterminationAlgorithm(decimal radius,
                                                     Camera &&cam,
                                                     size_t minimumIterations,
                                                     decimal distanceRatio,
                                                     decimal discriminatorRatio)
      : SphericalDistanceDeterminationAlgorithm(radius, std::forward<Camera>(cam)),
        minimumIterations_(minimumIterations),
        distanceRatioSq_(distanceRatio * distanceRatio),
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
     * point when projected onto the unit sphere (normalized).
     * It still functions the same way.
     */
    decimal GenerateLoss(PositionVector &position,
                         decimal targetDistanceSq,
                         decimal targetRadiusSq,
                         std::unique_ptr<Vec3[]> &projectedPoints,
                         size_t size);
    /**
     * Shuffles the indexes into triplets, attempting to create
     * triplets whose indicies are far from each other.
     * 
     * @param size The size of indicies, or how many indicies
     * to generate
     * @param n The end of the range to generate indicies for
     * @param indicies The array to write into
     * 
     * @pre Any precondition from this->Run
     * @pre size > 0 && size % 3 == 0
     * 
     * @post for any i, 0 <= indicies[i] < n
     * 
     * @note This algorithm uses a quadratic distribution to
     * prioritize points far away from a given index. We like that
     * because it helps deal with noise. To exaggerate the difference
     * in probability between points, you can use a function that grows
     * much faster by changing the macro PDF which is defined within.
     * Make sure the distribution function though is zero where you
     * need it to be (i.e. At points already generated within a triplet
     * so that you do not draw those points again)
     * 
     * @note The assumption of p from this->Run(p) being in polar
     * order is quite important in this algorithm. Should that not
     * be true, instead of using index differences in our quadratic
     * distribution, we'd need to instead use the distance between
     * the pixels corresponding to those indicies. This is not a
     * terrible change in terms of code, but is more compuationally
     * complex
     */
    void Shuffle(size_t size, size_t n, std::unique_ptr<size_t[]> &indicies);
    /// The minimum number of iterations to use
    size_t minimumIterations_;
    /// The maximum distance ratio to accept
    decimal distanceRatioSq_;
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
