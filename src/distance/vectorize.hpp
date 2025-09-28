#ifndef SRC_DISTANCE_VECTORIZE_HPP_
#define SRC_DISTANCE_VECTORIZE_HPP_

#include "common/spatial/attitude-utils.hpp"
#include "common/style.hpp"
#include "common/pipeline/stages.hpp"

namespace found {

/**
 * The VectorGenerationAlgorithm class houses the Vector Assembly Algorithm. This algorithm
 * finds the position from Earth with respect to its center with a 3D Vector (Vec3).
 * 
*/
class VectorGenerationAlgorithm : public FunctionStage<PositionVector, PositionVector> {
 public:
    // Constructs this
    VectorGenerationAlgorithm() = default;
    // Destroys this
    virtual ~VectorGenerationAlgorithm() {}
};

/**
 * The LOSTVectorGenerationAlgorithm class houses the a Vector Assembly Algorithm that calculates the
 * position of the satellite using orientation information determined from LOST.
 * 
*/
class LOSTVectorGenerationAlgorithm : public VectorGenerationAlgorithm {
 public:
    /**
     * Creates a LOSTVectorGenerationAlgorithm object
     * 
     * @param relativeOrientation The orientation of the FOUND camera with respect to the reference Orientation
     * @param referenceOrientation The orientation of the reference orientation
     * 
     * @pre You must use a forward rotation quaternion here. Remember that
     * forwards and backwards quaternions are conjugates.
    */
    explicit LOSTVectorGenerationAlgorithm(Quaternion relativeOrientation, Quaternion referenceOrientation)
        : orientation(relativeOrientation * referenceOrientation) {}

    /**
     * Creates a LOSTVectorGenerationAlgorithm object
     * 
     * @param orientation The absolute orientation of the FOUND camera
     * 
     * @pre You must use a forward rotation quaternion here. Remember that
     * forwards and backwards quaternions are conjugates.
    */
    explicit LOSTVectorGenerationAlgorithm(Quaternion orientation)
    : orientation(orientation) {}

    // Destroys this
    ~LOSTVectorGenerationAlgorithm() = default;

    /**
     * Runs the Vector Assembly Algorithm, which finds the vector of the satellite with respect
     * to Earth's center using information from LOST
     * 
     * @pre This class was initialized by forward quaternion(s)
     * 
     * @param x_E The distance from Earth
     * 
     * @return A PositionVector that represents the 3D Vector of the satellite relative to
     * Earth's center
    */
    PositionVector Run(const PositionVector &x_E) override;

 private:
    // Fields specific to this algorithm go here, and helper methods

    /// Orientation from LOST
    Quaternion orientation;
};

/**
 * FeatureDetectionVectorGenerationAlgorithm figures out
 * the distance vector of the satellite relative to earth
 * by identifying features on earth.
 */
class FeatureDetectionVectorGenerationAlgorithm : public VectorGenerationAlgorithm {
 public:
    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    FeatureDetectionVectorGenerationAlgorithm(/*Params to initialze fields for this object*/);

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    ~FeatureDetectionVectorGenerationAlgorithm();

    /**
     * Place documentation here. Press enter to automatically make a new line
     * 
     * @param x_E The position vector of the satellite with respect to the
     * camera coordiinate system
     * 
     * @return The position vector of the satellite with respect to the
     * planet's inertial reference frame
     * */
    PositionVector Run(const PositionVector &x_E) override;
 private:
    // Fields specific to this algorithm go here, and helper methods
};

}  // namespace found

#endif  // SRC_DISTANCE_VECTORIZE_HPP_
