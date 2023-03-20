#include "camera.hpp"

#include <math.h>
#include <assert.h>

namespace found {

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
Vec2 Camera::SpatialToCamera(const Vec3 &vector) const {
    // can't handle things behind the camera.
    assert(vector.x > 0);
    // TODO: is there any sort of accuracy problem when vector.y and vector.z are small?

    decimal focalFactor = focalLength/vector.x;

    decimal yPixel = vector.y*focalFactor;
    decimal zPixel = vector.z*focalFactor;

    return { -yPixel + xCenter, -zPixel + yCenter };
}

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
Vec3 Camera::CameraToSpatial(const Vec2 &vector) const {
    assert(InSensor(vector));

    // isn't it interesting: To convert from center-based to left-corner-based coordinates is the
    // same formula; f(x)=f^{-1}(x) !
    decimal xPixel = -vector.x + xCenter;
    decimal yPixel = -vector.y + yCenter;

    return {
        1,
        xPixel / focalLength,
        yPixel / focalLength,
    };
}

/**
 * Evaluates whether a vector can be seen in the camera
 * 
 * @param vector The vector to evaluate
 * 
 * @return true iff vector could be seen in this camera
*/
bool Camera::InSensor(const Vec2 &vector) const {
    // if vector.x == xResolution, then it is at the leftmost point 
    // of the pixel that's "hanging off" the edge of the image, 
    // so vector is still in the image.
    return vector.x >= 0 && vector.x <= xResolution
        && vector.y >= 0 && vector.y <= yResolution;
}

/**
 * Provides the Field of View (FOV) of this Camera
 * 
 * @return The FOV of this, in radians
 * 
*/
decimal Camera::Fov() const {
    return FocalLengthToFov(focalLength, xResolution, 1.0);
}

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
decimal FovToFocalLength(decimal xFov, decimal xResolution) {
    return xResolution / 2.0f / tan(xFov/2);
}

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
decimal FocalLengthToFov(decimal focalLength, decimal xResolution, decimal pixelSize) {
    return atan(xResolution/2 * pixelSize / focalLength) * 2;
}

}
