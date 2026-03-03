#ifndef SRC_DISTANCE_VECTORIZE_HPP_
#define SRC_DISTANCE_VECTORIZE_HPP_

#include "common/spatial/attitude-utils.hpp"
#include "common/spatial/camera.hpp"
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
     * @param relativeOrientation The orientation of the FOUND camera with respect to the reference orientation.
     * This is a backwards (celestial-to-camera) rotation quaternion.
     * @param referenceOrientation The orientation of the reference frame relative to the celestial frame.
     * This is also a backwards (celestial-to-camera) rotation quaternion.
     * @param cameraCelestialCoordinateOffset The difference between the camera coordinate definiont and the
     * celestial frame when the camera boresight is point at the celestial north pole. This is a rotation
     * from the celestial frame into the camera coordinate definition. Which for the current definion-- 
     * y points to the bottom of the image, x points to the right, and z points outward along the boresight-- 
     * is a 90 degree counter-clockwise rotation about the celesital norht pole.
     * 
    */
    explicit LOSTVectorGenerationAlgorithm(Quaternion relativeOrientation, Quaternion referenceOrientation,
                                           Quaternion cameraCelestialCoordinateOffset)
        : LOSTVectorGenerationAlgorithm(relativeOrientation * referenceOrientation, cameraCelestialCoordinateOffset) {}

    /**
     * Creates a LOSTVectorGenerationAlgorithm object
     * 
     * @param orientation The absolute orientation of the FOUND camera — a backwards
     *        (celestial-to-camera) rotation quaternion.
     * @param cameraCelestialCoordinateOffset The difference between the camera coordinate definiont and the
     * celestial frame when the camera boresight is point at the celestial north pole. This is a rotation
     * from the celestial frame into the camera coordinate definition. Which for the current definion-- 
     * y points to the bottom of the image, x points to the right, and z points outward along the boresight-- 
     * is a 90 degree counter-clockwise rotation about the celesital norht pole.
     * 
     * @pre orientation must be a backwards rotation quaternion (celestial → camera).
     *      Backwards and forwards quaternions are conjugates of each other.
     * 
     * @note The stored orientation is a forwards (camera → celestial) rotation built from
     *       two sequential rotations applied left-to-right:
     *       1. cam.GetRotationIntoCelestialFrame() — corrects for physical camera mounting,
     *          mapping the camera frame into the nominal (unmounted) camera frame
     *       2. orientation.conjugate()             — maps the nominal camera frame into the
     *          celestial frame
     *       In Run(), orientation is applied directly to the input PositionVector.
    */
    explicit LOSTVectorGenerationAlgorithm(Quaternion orientation, Quaternion cameraCelestialCoordinateOffset)
        : orientation(orientation * cameraCelestialCoordinateOffset) {}

    // Destroys this
    ~LOSTVectorGenerationAlgorithm() = default;

    /**
     * Runs the Vector Assembly Algorithm, which finds the vector of the satellite with respect
     * to Earth's center using information from LOST
     * 
     * @pre This class was initialized by backwards quaternion(s)
     * 
     * @param x_E The distance from Earth
     * 
     * @return A PositionVector that represents the 3D Vector of the satellite relative to
     * Earth's center
    */
    PositionVector Run(const PositionVector &x_E) override;

 private:
    // Fields specific to this algorithm go here, and helper methods

    /// Combined orientation: camera mounting correction composed with the LOST-derived
    /// rotation. Stored as a forwards (camera-to-celestial) quaternion; applied directly
    /// in Run() to map the input PositionVector into the celestial frame.
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
