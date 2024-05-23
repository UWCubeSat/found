#ifndef ATTITUDE_UTILS_H
#define ATTITUDE_UTILS_H

#include <memory>

namespace found {

// For controlling floating-point type memory usage throughout the program
typedef float decimal;

// At first, I wanted to have two separate Attitude classes, one storing Euler angles and converting
// to Quaterinon, and another storing as Quaternion and converting to Euler. But abstract classes
// make everything more annoying, because you need vectors of pointers...ugh!

/**
 * A Vec2 is an immutable object that represents a 2D Vector
 * 
*/
struct Vec2 {
    const decimal x;
    const decimal y;

    // Magnitude

    decimal Magnitude() const;
    decimal MagnitudeSq() const;

    // Unit Vector of this

    Vec2 Normalize() const;

    // Operations

    decimal operator*(const Vec2 &) const;
    Vec2 operator*(const decimal &) const;
    Vec2 operator-(const Vec2 &) const;
    Vec2 operator+(const Vec2 &) const;
};

class Mat3; // define above so we can use in Vec3 class

/**
 * A Vec3 is a mutable object that represents a 3D Vector
 * 
*/
class Vec3 {
public:
    decimal x;
    decimal y;
    decimal z;

    //TODO: Implement this constructor
    /**
     * Construction of orientation vector, which should be a unit vector
     * 
     * @param de The declination of the vector to create
     * @param ra The right ascension of the vector to create
    */
    Vec3(decimal de, decimal ra);

    /**
     * Construction of vector with x, y, and z components
     * 
     * @param x The scalar value in the x direction of the vector to make
     * @param y The scalar value in the y direction of the vector to make
     * @param z The scalar value in the z direction of the vector to make
    */
    Vec3(decimal x, decimal y, decimal z) : x(x), y(y), z(z) {};

    /**
     * Default construction of the Vector
    */
    Vec3() {};

    // Magnitude

    decimal Magnitude() const;
    decimal MagnitudeSq() const;

    // Unit Vector

    Vec3 Normalize() const;

    // TODO: Accessor Methods

    decimal getRightAscension() const;
    decimal getDeclination() const;

    // Operations

    decimal operator*(const Vec3 &) const;
    Vec3 operator*(const decimal &) const;
    Vec3 operator*(const Mat3 &) const;
    Vec3 operator-(const Vec3 &) const;
    Vec3 CrossProduct(const Vec3 &) const;
    Mat3 OuterProduct(const Vec3 &) const;
};

/**
 * A Mat3 is a mutable object that represents a 3x3 Matrix
 * 
*/
class Mat3 {
public:
    decimal x[9];

    // Accessor

    decimal At(int i, int j) const;
    Vec3 Column(int) const;
    Vec3 Row(int) const;
    decimal Trace() const;
    decimal Det() const;

    // Operations

    Mat3 operator+(const Mat3 &) const;
    Mat3 operator*(const Mat3 &) const;
    Vec3 operator*(const Vec3 &) const;
    Mat3 operator*(const decimal &) const;

    // Transformations

    Mat3 Transpose() const;
    Mat3 Inverse() const;
};

// Identity Matrix

extern const Mat3 kIdentityMat3;

// Buffer-Vector Functions

long SerializeLengthVec3();
void SerializeVec3(const Vec3 &, unsigned char *);
Vec3 DeserializeVec3(const unsigned char *);

// Distance between Vectors

decimal Distance(const Vec2 &, const Vec2 &);
decimal Distance(const Vec3 &, const Vec3 &);

/**
 * An EulerAngle is a mutable Object representing Euler Angles of a 3D point
 * 
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

/**
 * A Quaternion is a mutable object that represents a Quaternion. A Quaternion
 * is a common way to represent rotations in 3D.
 * 
*/
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


/**
 * An Attitude is an immutable object that represents the orientation of a 3D point.
 * 
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

// Vector operations
Vec2 midpoint(const Vec2 &, const Vec2 &);
Vec3 midpoint(const Vec3 &, const Vec3 &);
Vec3 midpoint(const Vec3 &, const Vec3 &, const Vec3);

// DCM-Quaternion-Spherical Conversions

Mat3 QuaternionToDCM(const Quaternion &);
Quaternion DCMToQuaternion(const Mat3 &);
Quaternion SphericalToQuaternion(decimal ra, decimal dec, decimal roll);

// Spherical-Vector Conversions

Vec3 SphericalToSpatial(decimal ra, decimal de);
void SpatialToSpherical(const Vec3 &, decimal &ra, decimal &de);

// Angle Between Vectors

decimal Angle(const Vec3 &, const Vec3 &);
decimal AngleUnit(const Vec3 &, const Vec3 &);

// Angle Conversions

decimal RadToDeg(decimal);
decimal DegToRad(decimal);
decimal RadToArcSec(decimal);
decimal ArcSecToRad(decimal);

// TODO: quaternion and euler angle conversion, conversion between ascension/declination to rec9tu

}

#endif
