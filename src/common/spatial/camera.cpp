#include "common/spatial/camera.hpp"

#include <math.h>
#include <assert.h>

#include "common/spatial/attitude-utils.hpp"

namespace found {

///////////////////////////////////
////////// CAMERA CLASS ///////////
///////////////////////////////////

Vec2 Camera::SpatialToCamera(const Vec3 &vector) const {
    // can't handle things behind the camera.
    assert(vector.x > 0);
    // TODO: is there any sort of accuracy problem when vector.y and vector.z are small?

    decimal focalFactor = focalLength/vector.x/pixelSize;

    decimal yPixel = vector.y*focalFactor;
    decimal zPixel = vector.z*focalFactor;

    return { -yPixel + xCenter, -zPixel + yCenter };
}

Vec3 Camera::CameraToSpatial(const Vec2 &vector) const {
    assert(InSensor(vector));

    // isn't it interesting: To convert from center-based to left-corner-based coordinates is the
    // same formula; f(x)=f^{-1}(x) !
    decimal xPixel = -vector.x + xCenter;
    decimal yPixel = -vector.y + yCenter;

    return {
        1,
        xPixel * pixelSize / focalLength,
        yPixel * pixelSize / focalLength,
    };
}

bool Camera::InSensor(const Vec2 &vector) const {
    // if vector.x == xResolution, then it is at the leftmost point
    // of the pixel that's "hanging off" the edge of the image,
    // so vector is still in the image.
    return vector.x >= 0 && vector.x <= xResolution
        && vector.y >= 0 && vector.y <= yResolution;
}

decimal Camera::Fov() const {
    return FocalLengthToFov(focalLength, xResolution, 1.0);
}

///////////////////////////////////
////// CONVERSION FUNCTIONS ///////
///////////////////////////////////

decimal FovToFocalLength(decimal xFov, decimal xResolution) {
    return xResolution / DECIMAL(2.0) / tan(xFov/2);
}

decimal FocalLengthToFov(decimal focalLength, decimal xResolution, decimal pixelSize) {
    return atan(xResolution/2 * pixelSize / focalLength) * 2;
}

}  // namespace found
