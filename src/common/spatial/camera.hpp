#ifndef CAMERA_H
#define CAMERA_H

#include "common/spatial/attitude-utils.hpp"

#include "common/style.hpp"

namespace found {

/**
 * A Camera is a mutable object that represents a Camera. All camera dimensions
 * are in SI
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

    Vec2 SpatialToCamera(const Vec3 &) const;
    Vec3 CameraToSpatial(const Vec2 &) const;

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

// Conversions from FOV to Focal Length

decimal FovToFocalLength(decimal xFov, decimal xResolution);
decimal FocalLengthToFov(decimal focalLength, decimal xResolution, decimal pixelSize);

}  // namespace found

#endif  // CAMERA_H
