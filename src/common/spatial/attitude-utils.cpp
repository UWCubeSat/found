
#include "common/spatial/attitude-utils.hpp"

#include <math.h>
#include <assert.h>

namespace found {

///////////////////////////////////
///// VECTOR UTILITY FUNCTIONS ////
///////////////////////////////////

Vec2 Midpoint(const Vec2 &vec1, const Vec2 &vec2) {
    return (vec1 + vec2) / DECIMAL(2.0);
}

Vec3 Midpoint(const Vec3 &vec1, const Vec3 &vec2) {
    return (vec1 + vec2) / DECIMAL(2.0);
}

Vec3 Midpoint(const Vec3 &vec1, const Vec3 &vec2, const Vec3 &vec3) {
    return (vec1 + vec2 + vec3) / DECIMAL(3.0);
}

decimal Distance(const Vec2 &v1, const Vec2 &v2) {
    return (v1 - v2).norm();
}

decimal Distance(const Vec3 &v1, const Vec3 &v2) {
    return (v1 - v2).norm();
}

///////////////////////////////////
///////// QUATERNION HELPERS //////
///////////////////////////////////

/**
 * Converts a Quaternion to Euler Angles.
 *
 * Uses the z-y'-x'' convention matching the custom ToSpherical() implementation.
 */
EulerAngles QuaternionToSpherical(const Quaternion &q) {
    // Eigen stores (x, y, z, w) internally but accessors are .w(), .x(), .y(), .z()
    // Our convention: real=w, i=x, j=y, k=z
    decimal w = q.w(), x = q.x(), y = q.y(), z = q.z();

    decimal ra = atan2(2*(-w*z+x*y), 1-2*(y*y+z*z));
    if (ra < 0)
        ra += 2*DECIMAL_M_PI;
    decimal de = -asin(2*(-w*y-x*z));
    decimal roll = -atan2(2*(-w*x+y*z), 1-2*(x*x+y*y));
    if (roll < 0)
        roll += 2*DECIMAL_M_PI;

    return EulerAngles(ra, de, roll);
}

///////////////////////////////////
////// CONVERSION FUNCTIONS ///////
///////////////////////////////////

Mat3 QuaternionToDCM(const Quaternion &quat) {
    return quat.toRotationMatrix();
}

Quaternion DCMToQuaternion(const Mat3 &dcm) {
    return Quaternion(dcm);
}

Quaternion SphericalToQuaternion(decimal ra, decimal dec, decimal roll) {
    assert(roll >= DECIMAL(0.0) && roll <= 2*DECIMAL_M_PI);
    assert(ra >= DECIMAL(0.0) && ra <= 2*DECIMAL_M_PI);
    assert(dec >= -DECIMAL_M_PI && dec <= DECIMAL_M_PI);

    // when we are modifying the coordinate axes, the quaternion multiplication works so that the
    // rotations are applied from left to right. This is the opposite as for modifying vectors.

    // It is indeed correct that a positive rotation in our right-handed coordinate frame is in the
    // clockwise direction when looking down/through the axis of rotation. Just like the right hand
    // rule for magnetic field around a current-carrying conductor.
    Quaternion a(AngleAxis(ra, Vec3(0, 0, 1)));
    Quaternion b(AngleAxis(-dec, Vec3(0, 1, 0)));
    Quaternion c(AngleAxis(-roll, Vec3(1, 0, 0)));
    Quaternion result = (a*b*c).conjugate();
    assert(DECIMAL_ABS(result.squaredNorm() - 1) < DECIMAL(0.00001));
    return result;
}

}  // namespace found
