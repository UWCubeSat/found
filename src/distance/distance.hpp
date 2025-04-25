#ifndef DISTANCE_H
#define DISTANCE_H

#include "common/style.hpp"
#include "common/pipeline.hpp"

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
    virtual ~DistanceDeterminationAlgorithm();
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
    * Initializes this SphericalDistanceDeterminationAlgorithm
    * 
    * @param radius The radius of Earth to use
    */
    explicit SphericalDistanceDeterminationAlgorithm(float radius);
    ~SphericalDistanceDeterminationAlgorithm();

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    PositionVector Run(const Points &p) override;
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

#endif
