#include "common/spatial/camera.hpp"

#include <math.h>
#include <assert.h>

#include "common/spatial/attitude-utils.hpp"

namespace found {

///////////////////////////////////
////////// CAMERA CLASS ///////////
///////////////////////////////////

Camera::Camera(decimal focalLength,
               int xResolution, int yResolution,
               decimal xCenter, decimal yCenter,
               decimal xPixelPitch, decimal yPixelPitch)
    : focalLength(focalLength),
      xResolution(xResolution), yResolution(yResolution),
      xCenter(xCenter), yCenter(yCenter),
      xPixelPitch(xPixelPitch), yPixelPitch(yPixelPitch),
      calibrationMatrix_(initCalibrationMatrix()),
      inverseCalibrationMatrix_(calibrationMatrix_.inverse()) {}

Camera::Camera(decimal focalLength, decimal pixelSize,
               int xResolution, int yResolution)
    : Camera(focalLength, xResolution, yResolution,
             xResolution / DECIMAL(2.0), yResolution / DECIMAL(2.0),
             pixelSize, pixelSize) {}

Mat3 Camera::initCalibrationMatrix() {
    decimal dx = focalLength / xPixelPitch;
    decimal dy = focalLength / yPixelPitch;

    // Compute the calibration matrix
    return Mat3 <<  xCenter     , -dy         , DECIMAL(0.0),
                    yCenter     , DECIMAL(0.0), -dx         ,
                    DECIMAL(1.0), DECIMAL(0.0), DECIMAL(0.0);
}

Vec2 Camera::SpatialToPixelCoordinates(const Vec3 &vector) const {
    // can't handle things behind the camera.
    assert(vector.z() > 0);
    // use similiar triangles to get the image coordinates
    Vec3 homogenousImageCoordinates = Vec3(vector.x() / vector.z(), vector.y() / vector.z(), DECIMAL(1.0));
    // transform image coordinates to pixel coordinates using the calibration matrix
    Vec3 homogenousPixelCoordinates = calibrationMatrix * homogenousImageCoordinates;

    return Vec2(homogenousPixelCoordinates.x(), homogenousPixelCoordinates.y());
}

Vec3 Camera::PixelToImageCoordinates(const Vec2 &vector) const {
    assert(InSensor(vector));
    return inverseCalibrationMatrix * Vec3(vector.x(), vector.y(), DECIMAL(1.0));
}

bool Camera::InSensor(const Vec2 &vector) const {
    // if vector.x == xResolution, then it is at the rightmost point
    // of the pixel that's "hanging off" the edge of the image,
    // so vector is still in the image.
    return vector.x() >= 0 && vector.x() <= xResolution
        && vector.y() >= 0 && vector.y() <= yResolution;
}

}  // namespace found
