
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

EulerAngles QuaternionToSpherical(const Quaternion &q) {
    // q is a world→camera quaternion built as (Qz(ra)*Qy(π/2−dec)*Qz(−roll))†.
    // Roll is a positive counter-clockwise rotation about the camera boresight (z-axis).
    // Decompose q† (camera→world) as a Z-Y-Z Euler sequence with angles (ra, π/2−dec, −roll).
    decimal w = q.w(), x = q.x(), y = q.y(), z = q.z();

    // de = asin(R_q†[2][2]) where R_q†[2][2] = 1−2(x²+y²) = sin(dec)
    decimal de = asin(1 - 2*(x*x + y*y));

    // Gimbal-lock at the poles: at the north pole only (ra − roll) is recoverable,
    // at the south pole only (ra + roll). Convention: set ra=0 and deduce roll.
    if (DECIMAL_ABS(de - DECIMAL_M_PI/2) < DECIMAL_TOLERANCE) {
        // North pole singularity: roll equals the physical Z-rotation angle.
        decimal roll = 2*atan2(z, w);
        if (roll < 0) roll += 2*DECIMAL_M_PI;
        return EulerAngles(DECIMAL(0.0), de, roll);
    }
    if (DECIMAL_ABS(de + DECIMAL_M_PI/2) < DECIMAL_TOLERANCE) {
        // South pole singularity: roll is negated relative to the Z-rotation.
        decimal roll = -2*atan2(x, y);
        if (roll < 0) roll += 2*DECIMAL_M_PI;
        return EulerAngles(DECIMAL(0.0), de, roll);
    }

    // General case: extract ra from R_q†[1][2] and R_q†[0][2]
    decimal ra = atan2(y*z + w*x, x*z - w*y);
    if (ra < 0) ra += 2*DECIMAL_M_PI;

    // Extract roll (note: first atan2 argument is negated vs the Z-Y-Z third angle)
    decimal roll = atan2(w*x - y*z, -(x*z + w*y));
    if (roll < 0) roll += 2*DECIMAL_M_PI;

    return EulerAngles(ra, de, roll);
}

Quaternion SphericalToQuaternion(decimal ra, decimal dec, decimal roll) {
    assert(roll >= DECIMAL(0.0) && roll <= 2*DECIMAL_M_PI);
    assert(ra >= DECIMAL(0.0) && ra <= 2*DECIMAL_M_PI);
    assert(dec >= -DECIMAL_M_PI/2 && dec <= DECIMAL_M_PI/2);

    // Build the camera→world rotation using intrinsic body-frame
    // rotations applied left to right:
    //   a: yaw by RA about Z (north-pole axis)
    //   b: pitch by (π/2 − dec) about Y
    //   c: negate roll about Z so that positive roll = CCW around boresight
    Quaternion qRa(AngleAxis(ra,                    Vec3(0, 0, 1)));
    Quaternion qDec(AngleAxis(DECIMAL_M_PI/2-dec,   Vec3(0, 1, 0)));
    Quaternion qRoll(AngleAxis(-roll,               Vec3(0, 0, 1)));

    // Conjugate gives world→camera rotation
    Quaternion result = qRa * qDec * qRoll;
    assert(DECIMAL_ABS(result.squaredNorm() - 1) < DECIMAL(0.00001));
    return result.conjugate();
}

}  // namespace found
