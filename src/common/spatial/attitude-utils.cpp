
#include "common/spatial/attitude-utils.hpp"

#include <math.h>
#include <assert.h>

namespace found {

///////////////////////////////////
<<<<<<< HEAD
///////// VECTOR CLASSES //////////
///////////////////////////////////

decimal Vec2::Magnitude() const {
    return sqrt(MagnitudeSq());
}

decimal Vec2::MagnitudeSq() const {
    return x*x+y*y;
}

Vec2 Vec2::Normalize() const {
    decimal mag = Magnitude();
    return {
        x/mag, y/mag,
    };
}

decimal Vec2::operator*(const Vec2 &other) const {
    return x*other.x + y*other.y;
}

Vec2 Vec2::operator*(const decimal &scalar) const {
    return { x*scalar, y*scalar };
}

Vec2 Vec2::operator+(const Vec2 &other) const {
    return {x + other.x, y + other.y };
}

Vec2 Vec2::operator-(const Vec2 &other) const {
    return { x - other.x, y - other.y };
}

Vec2 Vec2::operator-() const {
    return { -x, -y };
}

Vec2 Vec2::operator/(const decimal &scalar) const {
    return { x / scalar, y / scalar };
}

decimal Vec3::Magnitude() const {
    return sqrt(MagnitudeSq());
}

decimal Vec3::MagnitudeSq() const {
    return x*x+y*y+z*z;
}

Vec3 Vec3::Normalize() const {
    decimal mag = Magnitude();
    return {
        x/mag, y/mag, z/mag,
    };
}

Vec3 Vec3::operator-() const {
    return { -x, -y, -z };
}

Vec3 Vec3::operator-(const Vec3 &other) const {
    return { x - other.x, y - other.y, z - other.z };
}


decimal Vec3::operator*(const Vec3 &other) const {
    return x*other.x + y*other.y + z*other.z;
}

Vec3 Vec3::operator*(const decimal &scalar) const {
    return { x*scalar, y*scalar, z*scalar };
}

Vec3 Vec3::operator/(const decimal &divisor) const {
    return { x / divisor, y / divisor, z / divisor };
}

Vec3 &Vec3::operator+=(const Vec3 &other) {
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;

    return *this;
}

Vec3 Vec3::operator*(const Mat3 &other) const {
    return {
        x*other.At(0,0) + y*other.At(0,1) + z*other.At(0,2),
        x*other.At(1,0) + y*other.At(1,1) + z*other.At(1,2),
        x*other.At(2,0) + y*other.At(2,1) + z*other.At(2,2),
    };
}

Vec3 Vec3::CrossProduct(const Vec3 &other) const {
    return {
        y*other.z - z*other.y,
        -(x*other.z - z*other.x),
        x*other.y - y*other.x,
    };
}

Mat3 Vec3::OuterProduct(const Vec3 &other) const {
    return {
        x*other.x, x*other.y, x*other.z,
        y*other.x, y*other.y, y*other.z,
        z*other.x, z*other.y, z*other.z
    };
}

///////////////////////////////////
=======
>>>>>>> origin/eigen
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
    // prevent gimbal lock at the polls by only allowing roll
    assert(DECIMAL_ABS(dec) != DECIMAL_M_PI/2 || ra == 0);

    // Build the camera -> equatorial rotation using intrinsic body-frame
    // rotations applied left to right:
    //   qRa: yaw by RA about Z gloabl (north-pole axis)
    //   qDec: pitch by (π/2 − dec) about Y'
    //   qRoll: negate roll about X'' so that positive roll = CCW around boresight
    // sucessive rotations are around the new axes not the global axis.
    Quaternion qRa(AngleAxis(ra,      Vec3(0, 0, 1)));
    Quaternion qDec(AngleAxis(-dec,   Vec3(0, 1, 0)));
    Quaternion qRoll(AngleAxis(roll, Vec3(1, 0, 0)));
    Quaternion rotation = qRa * qDec * qRoll;

    // return: equatorial reference frame -> camera
    return rotation;
}

}  // namespace found
