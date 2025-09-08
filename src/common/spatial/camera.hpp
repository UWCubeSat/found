#ifndef SRC_COMMON_SPATIAL_CAMERA_HPP_
#define SRC_COMMON_SPATIAL_CAMERA_HPP_

#include "common/spatial/attitude-utils.hpp"

#include "common/style.hpp"

namespace found {

///////////////////////////////////
////////// CAMERA CLASS ///////////
///////////////////////////////////

/**
 * A Camera is a mutable object that represents a Camera.
 * 
 * @pre Camera dimensions are in SI
 * 
 * @note This object contains enough information to reconstruct a Camera Matrix
 * 
*/
class Camera {
 public:
    /**
     * Copy Constructor for Camera
     */
    Camera(const Camera &) = default;

    /**
     * Creates a Camera object off of real Camera parameters
     * 
     * @param focalLength The focal length of the camera
     * @param pixelSize The physical size of a pixel (in meters)
     * @param xCenter,yCenter The "principal point" of the camera. 
     * @param xResolution The resolution of the camera in the x-direction
     * @param yResolution The resolution of the camera in the y-direction
     */
    Camera(decimal focalLength, decimal pixelSize,
           decimal xCenter, decimal yCenter,
           int xResolution, int yResolution)
        : focalLength(focalLength), pixelSize(pixelSize),
          xCenter(xCenter), yCenter(yCenter),
          xResolution(xResolution), yResolution(yResolution) {}

    /**
     * Creates a Camera object off of ideal Camera parameters
     * 
     * @param focalLength The focal length of the camera
     * @param pixelSize The physical size of a pixel (in meters)
     * @param xResolution The resolution of the camera in the x-direction
     * @param yResolution The resolution of the camera in the y-direction
     * 
     * @note In an ideal camera, the x and y centers are just half the resolution, 
     * but physical cameras often have a bit of offset.
     */
    Camera(decimal focalLength, decimal pixelSize, int xResolution, int yResolution)
        : Camera(focalLength, pixelSize,
                 xResolution / (decimal) 2.0, yResolution / (decimal) 2.0,
                 xResolution, yResolution) {}

    // Projection of vectors into image and space

    /**
    * Converts from a 3D point in space to a 2D point on the camera sensor.
    * 
    * @param vector A 3D vector to convert to a vector on the camera
    * 
    * @return The 2D Vector that represents the 3D vector on the camera
    * 
    * @note Assumes that X is the depth direction and that it points away 
    * from the center of the sensor, i.e., any vector (x, 0, 0) will be at 
    * (xResolution/2, yResolution/2) on the sensor.
    */
    Vec2 SpatialToCamera(const Vec3 &) const;

    /**
    * Gives a point in 3d space that could correspond to the given vector, using the same
    * coordinate system described for SpatialToCamera.
    * 
    * @param vector The vector on the camera to convert to a 3D vector
    * 
    * @return A vector in 3d space corresponding to the given vector, with x-component equal to 1
    * 
    * @note Not all vectors returned by this function will necessarily have the same magnitude.
    * 
    * @warning Other functions rely on the fact that returned vectors are placed one unit away (x-component equal to 1). Don't change this behavior!
    */
    Vec3 CameraToSpatial(const Vec2 &) const;

    /**
    * Evaluates whether a vector can be seen in the camera
    * 
    * @param vector The vector to evaluate
    * 
    * @return true iff vector could be seen in this camera
   */
    bool InSensor(const Vec2 &vector) const;

    // Accessor Methods to Camera Parameters

   /**
     * Returns the X resolution of this camera
     * 
     * @return the X resolution of this
     */
    int XResolution() const { return xResolution; }

    /**
     * Returns the Y resolution of this camera
     * 
     * @return the Y resolution of this
     */
    int YResolution() const { return yResolution; }

    /**
     * Returns the focal length of this camera
     * 
     * @return The focal length of this
     */
    decimal FocalLength() const { return focalLength; }

    /**
     * Returns the pixel size of this camera
     * 
     * @return The pixel size of this
     */
    decimal PixelSize() const { return pixelSize; }

    /**
    * Provides the Field of View (FOV) of this Camera
    * 
    * @return The FOV of this, in radians
    * 
   */
    decimal Fov() const;

    // Mutator Method for Cameras

    /**
     * Sets the focal length of this
     * 
     * @param focalLength The focal length to give this camera
     */
    void SetFocalLength(decimal focalLength) { this->focalLength = focalLength; }

 private:
    // TODO: distortion
    /// The focal length (m)
    decimal focalLength;
    /// The pixel size (m)
    decimal pixelSize;
    /// The x center (pixels)
    decimal xCenter;
    /// The y center (pixels)
    decimal yCenter;
    /// The x resolution (pixels)
    int xResolution;
    /// The y resolution (pixels)
    int yResolution;
};

///////////////////////////////////
////// CONVERSION FUNCTIONS ///////
///////////////////////////////////

/**
 * Provides the focal length of a camera for given parameters
 * 
 * @param xFov The horizontal field of view
 * @param xResolution The horizontal resolution
 * 
 * @return The focal length of a camera with a given xFov and
 * xResolution
 * 
*/
decimal FovToFocalLength(decimal xFov, decimal xResolution);

/**
 * Provides the FOV of a camera for given parameters
 * 
 * @param focalLength The focal length
 * @param xResolution The horizontal resolution
 * @param pixelSize The size of a pixel in a Camera
 * 
 * @return The FOV of a camera with a given focalLength,
 * xResolution, and pixelSize
 * 
*/
decimal FocalLengthToFov(decimal focalLength, decimal xResolution, decimal pixelSize);

}  // namespace found

#endif  // SRC_COMMON_SPATIAL_CAMERA_HPP_
