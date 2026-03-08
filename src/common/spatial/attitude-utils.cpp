
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

Quaternion SphericalToQuaternion(decimal ra, decimal dec, decimal roll) {
    assert(roll >= DECIMAL(0.0) && roll <= 2*DECIMAL_M_PI);
    assert(ra >= DECIMAL(0.0) && ra <= 2*DECIMAL_M_PI);
    assert(dec >= -DECIMAL_M_PI/2 && dec <= DECIMAL_M_PI/2);

    // Build the camera→world rotation using intrinsic body-frame
    // rotations applied left to right:
    //   a: yaw by RA about Z (north-pole axis)
    //   b: pitch by (π/2 − dec) about Y
    //   c: negate roll about X so that positive roll = CCW around boresight WARNING wrong
    Quaternion qRa(AngleAxis(ra,      Vec3(0, 0, 1)));
    Quaternion qDec(AngleAxis(-dec,   Vec3(0, 1, 0)));
    Quaternion qRoll(AngleAxis(-roll, Vec3(1, 0, 0)));

    // Conjugate gives world→camera rotation
    Quaternion result = qRa * qDec * qRoll;
    assert(DECIMAL_ABS(result.squaredNorm() - 1) < DECIMAL_TOLERANCE);
    return result.conjugate();
}

}  // namespace found
