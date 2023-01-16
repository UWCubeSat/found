#ifndef CAMERA_H
#define CAMERA_H

#include "attitude-utils.hpp"

#include "style.hpp"

namespace found {

/**
 * A Camera is a mutable object that represents a Camera.
 * 
 * @note This object contains enough information to reconstruct a Camera Matrix
 * 
*/
class Camera {
public:
    Camera(const Camera &) = default;

    /**
     * Creates a Camera object off of real Camera parameters
     * 
     * @param focalLength The focal length of the camera
     * @param xCenter,yCenter The "principal point" of the camera. 
     * @param xResolution The resolution of the camera in the x-direction
     * @param yResolution The resolution of the camera in the y-direction
     */
    Camera(decimal focalLength,
           decimal xCenter, decimal yCenter,
           int xResolution, int yResolution)
        : focalLength(focalLength),
          xCenter(xCenter), yCenter(yCenter),
          xResolution(xResolution), yResolution(yResolution) {};

    /**
     * Creates a Camera object off of ideal Camera parameters
     * 
     * @param focalLength The focal length of the camera
     * @param xResolution The resolution of the camera in the x-direction
     * @param yResolution The resolution of the camera in the y-direction
     * 
     * @note In an ideal camera, the x and y centers are just half the resolution, 
     * but physical cameras often have a bit of offset.
     */
    Camera(decimal focalLength, int xResolution, int yResolution)
        : Camera(focalLength,
                 xResolution / (decimal) 2.0, yResolution / (decimal) 2.0,
                 xResolution, yResolution) {};

    // Projection of vectors into image and space

    Vec2 SpatialToCamera(const Vec3 &) const;
    Vec3 CameraToSpatial(const Vec2 &) const;

    bool InSensor(const Vec2 &vector) const;

    // Accessor Methods to Camera Parameters

    int XResolution() const { return xResolution; };
    int YResolution() const { return yResolution; };
    decimal FocalLength() const { return focalLength; };
    decimal Fov() const;

    // Mutator Method for Cameras

    void SetFocalLength(decimal focalLength) { this->focalLength = focalLength; }

private:
    // TODO: distortion
    decimal focalLength;
    decimal xCenter; decimal yCenter;
    int xResolution; int yResolution;
};

// Conversions from FOV to Focal Length

decimal FovToFocalLength(decimal xFov, decimal xResolution);
decimal FocalLengthToFov(decimal focalLength, decimal xResolution, decimal pixelSize);
}

#endif
