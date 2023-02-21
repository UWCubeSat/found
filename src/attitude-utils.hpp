#ifndef ATTITUDE_UTILS_H
#define ATTITUDE_UTILS_H

#include <memory>

#include "style.hpp"

namespace found {

// At first, I wanted to have two separate Attitude classes, one storing Euler angles and converting
// to Quaterinon, and another storing as Quaternion and converting to Euler. But abstract classes
// make everything more annoying, because you need vectors of pointers...ugh!

/// A two dimensional vector with floating point components
struct Vec2 {
    decimal x;
    decimal y;

    decimal Magnitude() const;
    decimal MagnitudeSq() const;

    Vec2 Normalize() const;

    decimal operator*(const Vec2 &) const;
    Vec2 operator*(const decimal &) const;
    Vec2 operator-(const Vec2 &) const;
    Vec2 operator+(const Vec2 &) const;
};

class Mat3; // define above so we can use in Vec3 class

/// Three dimensional vector with floating point components
class Vec3 {
public:
    decimal x;
    decimal y;
    decimal z;

    decimal Magnitude() const;
    decimal MagnitudeSq() const;
    Vec3 Normalize() const;

    decimal operator*(const Vec3 &) const;
    Vec3 operator*(const decimal &) const;
    Vec3 operator*(const Mat3 &) const;
    Vec3 operator-(const Vec3 &) const;
    Vec3 CrossProduct(const Vec3 &) const;
    Mat3 OuterProduct(const Vec3 &) const;
};

/// 3x3 vector with floating point components
class Mat3 {
public:
    decimal x[9];

    decimal At(int i, int j) const;
    Mat3 operator+(const Mat3 &) const;
    Mat3 operator*(const Mat3 &) const;
    Vec3 operator*(const Vec3 &) const;
    Mat3 operator*(const decimal &) const;
    Mat3 Transpose() const;
    Vec3 Column(int) const;
    Vec3 Row(int) const;
    decimal Trace() const;
    decimal Det() const;
    Mat3 Inverse() const;
};

extern const Mat3 kIdentityMat3;

long SerializeLengthVec3();
void SerializeVec3(const Vec3 &, unsigned char *);
Vec3 DeserializeVec3(const unsigned char *);

decimal Distance(const Vec2 &, const Vec2 &);
decimal Distance(const Vec3 &, const Vec3 &);

/**
 * A "human-readable" way to represent a 3d rotation or orientation.
 * Euler angles roughly correspond to yaw, pitch, and roll of an airplane, which are easy for humans to understand.
 * There's no one single way to store Euler angles. We use z-y'-x'' angles, according to the notation used on the wikipedia page for euler angles.
 */
class EulerAngles {
public:
    EulerAngles(decimal ra, decimal de, decimal roll)
        : ra(ra), de(de), roll(roll) { };

    /// Right ascension. How far we yaw left. Yaw is performed first.
    decimal ra;
    /// Declination. How far we pitch up (or down if negative). Pitch is performed second, after yaw.
    decimal de;
    /// How far we roll counterclockwise. Roll is performed last (after yaw and pitch).
    decimal roll;
};

/// A quaternion is a common way to represent a 3d rotation.
class Quaternion {
public:
    Quaternion() = default;
    explicit Quaternion(const Vec3 &);
    Quaternion(const Vec3 &, decimal);

    Quaternion(decimal real, decimal i, decimal j, decimal k)
        : real(real), i(i), j(j), k(k) { };

    Quaternion operator*(const Quaternion &other) const;
    Quaternion Conjugate() const;
    Vec3 Vector() const;
    void SetVector(const Vec3 &);
    Vec3 Rotate(const Vec3 &) const;
    decimal Angle() const;
    void SetAngle(decimal);
    EulerAngles ToSpherical() const;
    bool IsUnit(decimal tolerance) const;
    Quaternion Canonicalize() const;

    decimal real;
    decimal i;
    decimal j;
    decimal k;
};

//
/**
 * The attitude (orientation) of a spacecraft.
 * The Attitude object stores either a rotation matrix (direction cosine matrix) or a quaternion, and converts automatically to the other format when needed.
 * @note When porting to an embedded device, you'll probably want to get rid of this class and adapt to
 * either quaternions or DCMs exclusively, depending on the natural output format of whatever
 * attitude estimation algorithm you're using.
 */
class Attitude {
public:
    Attitude() = default;
    explicit Attitude(const Quaternion &); // NOLINT
    explicit Attitude(const Mat3 &dcm);

    Quaternion GetQuaternion() const;
    Mat3 GetDCM() const;
    EulerAngles ToSpherical() const;
    Vec3 Rotate(const Vec3 &) const;

private:
    enum AttitudeType {
        NullType,
        QuaternionType,
        DCMType,
    };

    Quaternion quaternion;
    Mat3 dcm; // direction cosine matrix
    AttitudeType type;
};

Mat3 QuaternionToDCM(const Quaternion &);
Quaternion DCMToQuaternion(const Mat3 &);

// Return a quaternion that will reorient the coordinate axes so that the x-axis points at the given
// right ascension and declination, then roll the coordinate axes counterclockwise (i.e., the stars
// will appear to rotate clockwise). This is an "improper" z-y'-x' Euler rotation.
Quaternion SphericalToQuaternion(decimal ra, decimal dec, decimal roll);

// returns unit vector
Vec3 SphericalToSpatial(decimal ra, decimal de);
void SpatialToSpherical(const Vec3 &, decimal *ra, decimal *de);
// angle between two vectors, using dot product and magnitude division
decimal Angle(const Vec3 &, const Vec3 &);
// angle between two vectors, /assuming/ that they are already unit length
decimal AngleUnit(const Vec3 &, const Vec3 &);

decimal RadToDeg(decimal);
decimal DegToRad(decimal);
decimal RadToArcSec(decimal);
decimal ArcSecToRad(decimal);

// TODO: quaternion and euler angle conversion, conversion between ascension/declination to rec9tu

}

#endif
