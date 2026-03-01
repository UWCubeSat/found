
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
////// CONVERSION FUNCTIONS ///////
///////////////////////////////////

Mat3 QuaternionToDCM(const Quaternion &quat) {
    return quat.toRotationMatrix();
}

Quaternion DCMToQuaternion(const Mat3 &dcm) {
    return Quaternion(dcm);
}

EulerAngles QuaternionToSpherical(const Quaternion &q) {
    // Undo the axis permutation (new camera → old camera) so we can
    // extract Euler angles using the original formulas.
    // kAxisPermutationInv = inverse of -120° rotation about (1,1,1)/sqrt(3)
    static const Quaternion kAxisPermutationInv(
        DECIMAL(0.5), DECIMAL(0.5), DECIMAL(0.5), DECIMAL(0.5));
    Quaternion qOld = kAxisPermutationInv * q;

    decimal w = qOld.w(), x = qOld.x(), y = qOld.y(), z = qOld.z();

    decimal ra = atan2(2*(-w*z+x*y), 1-2*(y*y+z*z));
    if (ra < 0)
        ra += 2*DECIMAL_M_PI;
    decimal de = -asin(2*(-w*y-x*z));
    decimal roll = -atan2(2*(-w*x+y*z), 1-2*(x*x+y*y));
    if (roll < 0)
        roll += 2*DECIMAL_M_PI;

    return EulerAngles(ra, de, roll);
}

Quaternion SphericalToQuaternion(decimal ra, decimal dec, decimal roll) {
    assert(roll >= DECIMAL(0.0) && roll <= 2*DECIMAL_M_PI);
    assert(ra >= DECIMAL(0.0) && ra <= 2*DECIMAL_M_PI);
    assert(dec >= -DECIMAL_M_PI/2 && dec <= DECIMAL_M_PI/2);

    // Build the forward rotation (camera → world) using intrinsic body-frame
    // rotations applied left to right:
    //   a: yaw by RA about Z (north-pole axis)
    //   b: pitch by -dec about Y
    //   c: roll by roll about X (line-of-sight in the intermediate frame)
    Quaternion qRa(AngleAxis(ra, Vec3(0, 0, 1)));
    Quaternion qDec(AngleAxis(DECIMAL_M_PI/2-dec, Vec3(0, 1, 0)));
    Quaternion qRoll(AngleAxis(roll, Vec3(1, 0, 0)));

    // world to camera coordinate rotation
    Quaternion result = qRoll * qRa * qDec;
    assert(DECIMAL_ABS(result.squaredNorm() - 1) < DECIMAL(0.00001));
    return result;
}

}  // namespace found
