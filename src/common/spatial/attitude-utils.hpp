#ifndef SRC_COMMON_SPATIAL_ATTITUDE_UTILS_HPP_
#define SRC_COMMON_SPATIAL_ATTITUDE_UTILS_HPP_

#include <memory>

#include "common/decimal.hpp"

namespace found {

// At first, I wanted to have two separate Attitude classes, one storing Euler angles and converting
// to Quaterinon, and another storing as Quaternion and converting to Euler. But abstract classes
// make everything more annoying, because you need vectors of pointers...ugh!

///////////////////////////////////
///////// VECTOR CLASSES //////////
///////////////////////////////////

/**
 * A Vec2 is an immutable object that represents a 2D Vector
 * 
*/
struct Vec2 {
    /// The x coordinate
    decimal x;
    /// The y coordinate
    decimal y;

    // Magnitude

    /**
     * Provides the magnitude of this Vec2
     * 
     * @return The magnitude of this
    */
    decimal Magnitude() const;

    /**
     * Provides the square of the magnitude of this Vec2
     * 
     * @return The square of the magnitude of this
    */
    decimal MagnitudeSq() const;

    // Unit Vector

    /**
     * Normalizes this
     * 
     * @return The normalized vector of this
     */
    Vec2 Normalize() const;

    // Vector Operations

    /**
     * Dot product (Scalar product)
     * 
     * @param other The other vector
     * 
     * @return The dot of this and other
     */
    decimal operator*(const Vec2 &other) const;

    /**
     * Scalar-vector Product
     * 
     * @param scalar The scalar
     * 
     * @return scalar * this
     */
    Vec2 operator*(const decimal &scalar) const;

    /**
     * Vector Addition
     * 
     * @param other The other vector
     * 
     * @return this + other
     */
    Vec2 operator+(const Vec2 &other) const;

    /**
     * Vector Subtraction
     * 
     * @param other The other vector
     * 
     * @return this - other
     */
    Vec2 operator-(const Vec2 &other) const;

    /**
     * Orthogonalization
     * 
     * @return The orthogonal vector to this
     */
    Vec2 Orthogonal() const;
};

class Mat3;  // define above so we can use in Vec3 class

/**
 * A Vec3 is a mutable object that represents a 3D Vector
 * 
*/
class Vec3 {
 public:
    /// The x coordinate
    decimal x;
    /// The y coordinate
    decimal y;
    /// The z coordinate
    decimal z;

    /**
     * Construction of vector with x, y, and z components
     * 
     * @param x The scalar value in the x direction of the vector to make
     * @param y The scalar value in the y direction of the vector to make
     * @param z The scalar value in the z direction of the vector to make
    */
    constexpr Vec3(decimal x, decimal y, decimal z) : x(x), y(y), z(z) {}

    /**
     * Default construction of the Vector
    */
    Vec3() = default;

    // Magnitude

    /**
     * Provides the magnitude of this Vec3
     * 
     * @return The magnitude of this
    */
    decimal Magnitude() const;

    /**
     * Provides the square of the magnitude of this Vec3
     * 
     * @return The square of the magnitude of this
    */
    decimal MagnitudeSq() const;

    // Unit Vector

    /**
     * Provides the magnitude of this Vec2
     * 
     * @return The magnitude of this
    */
    Vec3 Normalize() const;

    // Operations

    /**
     * Vector Negation
     * 
     * @return -this
     */
    Vec3 operator-() const;

    /**
     * Vector Subtraction
     * 
     * @param other The other vector
     * 
     * @return this - other
     */
    Vec3 operator-(const Vec3 &) const;

    /**
     * Dot Product
     * 
     * @param other The other vector
     * 
     * @return The dot of this and other
     */
    decimal operator*(const Vec3 &) const;

    /**
     * Scalar Product
     * 
     * @param scalar The scalar
     * 
     * @return scalar * this
     */
    Vec3 operator*(const decimal &) const;

    /**
     * Scalar Division
     * 
     * @param divisor The divisor
     * 
     * @return this / divisor
     */
    Vec3 operator/(const decimal &) const;

    /**
     * Vector Addition (Modification)
     * 
     * @param other The other vector
     * 
     * @return this
     * 
     * @post this has been added by other
     */
    Vec3 &operator+=(const Vec3 &);

    /**
     * Computes the product of a 3x3 matrix and a 3x1 vector (this)
     * 
     * @param other The matrix to multiply this with
     * 
     * @return other @ this
     * 
    */
    Vec3 operator*(const Mat3 &) const;

    /**
     * Computes the cross (vector) product between this and another vector
     * 
     * @param other The vector to cross with this
     * 
     * @return this X other
    */
    Vec3 CrossProduct(const Vec3 &) const;

    /**
     * Computes the outer product between this and another vector
     * 
     * @param other The other vector in this operation
     * 
     * @return this ⊗ other
    */
    Mat3 OuterProduct(const Vec3 &) const;
};

///////////////////////////////////
///// VECTOR UTILITY FUNCTIONS ////
///////////////////////////////////

/**
 * Finds the midpoint between two different vectors
 *
 * @param vec1 The first vector
 * @param vec2 The second vector
 *
 * @return The midpoint vector
*/
Vec2 Midpoint(const Vec2 &, const Vec2 &);

/**
 * Finds the midpoint between two different vectors
 *
 * @param vec1 The first vector
 * @param vec2 The second vector
 *
 * @return The midpoint vector
*/
Vec3 Midpoint(const Vec3 &, const Vec3 &);

/**
 * Finds the midpoint between three different vectors
 *
 * @param vec1 The first vector
 * @param vec2 The second vector
 * @param vec3 The third vector
 *
 * @return The midpoint vector
*/
Vec3 Midpoint(const Vec3 &, const Vec3 &, const Vec3 &);

/**
 * Determines the Distance between two vectors
 * 
 * @param v1 The first vector
 * @param v2 The second vector
 * 
 * @return The distance between v1 and v2
*/
decimal Distance(const Vec2 &, const Vec2 &);

/**
 * Determines the Distance between two vectors
 * 
 * @param v1 The first vector
 * @param v2 The second vector
 * 
 * @return The distance between v1 and v2
*/
decimal Distance(const Vec3 &, const Vec3 &);

/**
 * Determines the angle between two different vectors
 * 
 * @param vec1 The first vector
 * @param vec2 The second vector
 * 
 * @return The angle, in radians, between vec1 and vec2
*/
decimal Angle(const Vec3 &, const Vec3 &);

/**
 * Determines the angle between two different vectors
 * 
 * @param vec1 The first vector
 * @param vec2 The second vector
 * 
 * @return The angle, in radians, between vec1 and vec2
 * 
 * @pre The magnitude of vec1 and vec2 are 1
*/
decimal AngleUnit(const Vec3 &, const Vec3 &);

///////////////////////////////////
///////// MATRIX CLASS ////////////
///////////////////////////////////

/**
 * A Mat3 is a mutable object that represents a 3x3 Matrix
 * 
*/
class Mat3 {
 public:
    /// The matrix entries
    decimal x[9];

    // Accessor

    /**
    * Obtains an entry in this Matrix
    * 
    * @param i The row of the entry
    * @param j The column of the entry
    * 
    * @return The value of the entry in this at (i, j)
    */
    decimal At(int i, int j) const;

    /**
     * Obtains one of the column vectors in this Matrix
     * 
     * @param j The column of the vector
     * 
     * @return The vector at column j
    */
    Vec3 Column(int) const;

    /**
     * Obtains one of the row vectors in this Matrix
     * 
     * @param i The row of the vector
     * 
     * @return The vector at row i
    */
    Vec3 Row(int) const;

    /**
     * Obtains the trace of this Matrix
     * 
     * @return The trace of this
    */
    decimal Trace() const;

    /**
     * Obtains the determinant of this Matrix
     * 
     * @return The determinant of this
    */
    decimal Det() const;

    // Operations

    /**
     * Matrix Addition (element-wise)
     * 
     * @param other
     * 
     * @return this + other
     */
    Mat3 operator+(const Mat3 &) const;

    /**
     * Matrix Multiplication
     * 
     * @param other The other matrix
     * 
     * @return this @ other
     */
    Mat3 operator*(const Mat3 &) const;

    /**
     * Matrix-Vector Multiplication
     * 
     * @param vec The vector to multiply
     * 
     * @return this @ other
     * 
     * @note Same as Vector::operator*(const Mat3 &),
     * but with swapped parameters (duh?)
     */
    Vec3 operator*(const Vec3 &) const;

    /**
     * Matrix-Scalar Multiplication
     * 
     * @param scalar The scalar to multiply with
     * 
     * @return scalar * this
     */
    Mat3 operator*(const decimal &) const;

    // Transformations

    /**
     * Obtains the transpose of this Matrix
     * 
     * @return The transpose Matrix of this
     * 
     * @note Use this over Inverse if your
     * matrix is orthogonal (e.g. A DCM)
    */
    Mat3 Transpose() const;

    /**
     * Obtains the inverse of this Matrix
     * 
     * @return The inverse Matrix of this 
    */
    Mat3 Inverse() const;
};

/// Identity Matrix
extern const Mat3 kIdentityMat3;

///////////////////////////////////
//////// EULER ANGLES CLASS ///////
///////////////////////////////////

/**
 * An EulerAngle is a mutable Object representing Euler Angles of a 3D point
 * 
 * A "human-readable" way to represent a 3d rotation or orientation.
 * Euler angles roughly correspond to yaw, pitch, and roll of an airplane,
 * which are easy * for humans to understand.
 * 
 * There's no one single way to store Euler angles. We use z-y'-x'' angles,
 * according to * the notation used on the wikipedia page for euler angles.
 */
class EulerAngles {
 public:
    /**
     * Initializes an EulerAngles with corresponding parameters
     * 
     * @param ra The Right Ascension of the Euler Angle
     * @param de The Declination of the Euler Angle
     * @param roll The roll of the Euler Angle
     */
    constexpr EulerAngles(decimal ra = DECIMAL(0.0),
                decimal de = DECIMAL(0.0),
                decimal roll = DECIMAL(0.0))
        : ra(ra), de(de), roll(roll) {}

    /// Right ascension. How far we yaw left. Yaw is performed first.
    decimal ra;
    /// Declination. How far we pitch up (or down if negative). Pitch is performed second, after yaw.
    decimal de;
    /// How far we roll counterclockwise. Roll is performed last (after yaw and pitch).
    decimal roll;
};

///////////////////////////////////
///////// QUATERNION CLASS ////////
///////////////////////////////////

/**
 * A Quaternion is a mutable object that represents a Quaternion. A Quaternion
 * is a common way to represent rotations in 3D.
 * 
*/
class Quaternion {
 public:
    Quaternion() = default;

    /**
     * Create a quaternion which represents a rotation of theta around the axis input
     * 
     * @param input the axis about which theta is defined
     * @param theta the rotation about the input
    */
    Quaternion(const Vec3 &, decimal);

    /**
     * Creates a "pure quaternion" with the given vector for imaginary parts and zero for real part.
     * 
     * @param input The vector representing the imaginary part of the Quaternion to create
     * */
    explicit Quaternion(const Vec3 &);

    /**
     * Creates a Quaternion with components
     * 
     * @param real The real component
     * @param i The i component
     * @param j The j component
     * @param k The k component
     * 
     * Initializes this to be {real + iI + jJ + kK}
     */
    Quaternion(decimal real, decimal i, decimal j, decimal k)
        : real(real), i(i), j(j), k(k) {}

    /**
     * Multiply two quaternions using the usual definition of
     * quaternion multiplication (effectively composes rotations)
     * 
     * @param other The other quaternion
     * 
     * @return The resulting quaternion
     */
    Quaternion operator*(const Quaternion &other) const;

    /**
     * Negate the quaternion, which is the same rotation
     * 
     * @return The resulting quaternion
     */
    Quaternion operator-() const;

    /**
     * Effectively computes a quaternion representing the inverse
     * rotation of the original.
     * 
     * @return The resulting quaternion
     */
    Quaternion Conjugate() const;

    /**
     * @brief The axis of the represented rotation.
     * 
     * The vector formed by imaginary components of the quaternion.
     * 
     * @return The vector part of this quaternion
     */
    Vec3 Vector() const;

    /**
     * Set imaginary components of Quarterion.
     * 
     * @param vec The vector holding the imaginary quantities
     * */
    void SetVector(const Vec3 &);

    /**
     * Rotates a vector about this Quaternion
     * 
     * @param input The vector to rotate about this
     * 
     * @return The input vector that has been rotated about this
    */
    Vec3 Rotate(const Vec3 &) const;

    /**
     * Provides the amount of rotations represented by this Quaterion
     * 
     * @return The amount of rotations represented by this, in radians
    */
    decimal Angle() const;

    /**
     * Changes the rotation represented in this quarterion
     * 
     * @param newAngle The rotation angle this new quarternion should represent
     * 
     * @post Changes this by altering the rotation angle of this by newAngle
    */
    void SetAngle(decimal);

    /**
     * Tells client if this Quarternion is a unit quaternion
     * 
     * @param tolerance The tolerance for the magnitude of this
     * 
     * @return true iff this is a unit Quaternion
     * 
    */
    bool IsUnit(decimal tolerance) const;

    /**
     * Provides the canonicalized Quaternion of this, if it is not already
     * canonicalized
     * 
     * @return The canonicalized Quaternion of this, which is this iff this
     * is a canonicalized Quaternion already, and a new Quaternion if it is
     * not
     */
    Quaternion Canonicalize() const;

    /**
     * Converts this Quarterion to a new system of Euler Angles by extracting the
     * rotations described by this quarterion
     * 
     * @return An EulerAngles object representing this in with Euler Angles
    */
    EulerAngles ToSpherical() const;

    /// The real component
    decimal real;
    /// The i component
    decimal i;
    /// The j component
    decimal j;
    /// The k component
    decimal k;
};

///////////////////////////////////
///////// ATTITUDE CLASS //////////
///////////////////////////////////

/**
 * An Attitude is an immutable object that represents the orientation of a 3D point.
 * Otherwise the attitude (orientation) of a spacecraft.
 * 
 * @note This class wraps DCMs or Quaternions as the orientation
 * 
 * @note The Attitude object stores either a rotation matrix (direction cosine matrix)
 * or a quaternion, and converts automatically to the other format when needed.
 * 
 * @note When porting to an embedded device, you'll probably want to get rid of this class and adapt to
 * either quaternions or DCMs exclusively, depending on the natural output format of whatever
 * attitude estimation algorithm you're using.
 */
class Attitude {
 public:
    /**
     * Constructs this by default
     */
    Attitude() = default;

    /**
     * Constructs an Attitude object from Quaternion information
     * 
     * @param quat The quaternion to base the attitude off of  
    */
    explicit Attitude(const Quaternion &quat) : quaternion(quat), type(QuaternionType) {}

    /**
     * Constructs an Attitude object from a Direction Cosine Matrix (A
     * matrix holding the direction cosines for an attitude)
     * 
     * @param matrix The matrix holding the direction cosines
    */
    explicit Attitude(const Mat3 &matrix) : dcm(matrix), type(DCMType) {}

    /**
     * Provides the Quaternion corresponding to this Attitude
     * 
     * @return A Quaternion that holds the attitude information
     * of this
    */
    Quaternion GetQuaternion() const;

    /**
     * Obtains the rotation matrix from this Attitude
     * 
     * @return A matrix containing the direction cosines
     * indicated by this
    */
    Mat3 GetDCM() const;

    /**
     * Obtains the Euler Angles of this Attitude
     * 
     * @return An EulerAngles object that holds the Euler
     * Angles of this
    */
    EulerAngles ToSpherical() const;

    /**
     * Obtains a vector rotated to this Attitude from another vector 
     * (Convert a vector from the reference frame to the body frame.)
     * 
     * @param vec The vector to rotate
     * 
     * @return A new vector that is rotated from vec based on this
    */
    Vec3 Rotate(const Vec3 &) const;

 private:
    /// Represents what internal representation for attitude
    /// is being used
    enum AttitudeType {
        /// No Internal representation
        NullType,
        /// Quaternion Internal Representation
        QuaternionType,
        /// DCM Internal Representation
        DCMType,
    };

    /// Internal Quaternion Representation
    Quaternion quaternion;
    /// Internal DCM Representation
    Mat3 dcm;
    /// Internal Representation Type
    AttitudeType type;
};

///////////////////////////////////
////// CONVERSION FUNCTIONS ///////
///////////////////////////////////


// Attitude Conversions


/**
 * Creates a Direction Cosine Matrix (DCM) off of a Quaternion.
 * 
 * @param quat The quaternion to base the DCM off of
 * 
 * @return A Matrix holding the direction cosines of a particular
 * attitude (orientation)
 * 
 * @note A DCM is also a rotation matrix. If B is a DCM, multiplying
 * B by Vector v will result in vector u where u is v rotated to the
 * angles that the direction cosines hold.
*/
Mat3 QuaternionToDCM(const Quaternion &);

/**
 * Creates a Quaternion based on a Direction Cosine Matrix (rotation matrix)
 * 
 * @param dcm The matrix holding the direction cosines
 * 
 * @return A Quaternion that expresses the rotation defined in dcm
*/
Quaternion DCMToQuaternion(const Mat3 &);

/**
 * Converts Euler Angles into a quaternion
 * 
 * @param ra The right ascension of the Euler Angles
 * @param dec The declination of the Euler Angles
 * @param roll The roll of the Euler Angles
 * 
 * @return A Quaternion representing this collection of Euler Angles
 * 
 * @note Returned Quaternion will reorient the coordinate axes so that the x-axis points at the given
 * right ascension and declination, then roll the coordinate axes counterclockwise (i.e., the stars
 * will appear to rotate clockwise). This is an "improper" z-y'-x' Euler rotation.
 * 
 * @note Rotating a vector with this quaternion is equivalent to a forwards rotation (rotation into
 * the absolute frame)
*/
Quaternion SphericalToQuaternion(decimal ra, decimal dec, decimal roll);

/**
 * Converts Euler Angles into a quaternion
 * 
 * @param angles The euler angles to convert
 * 
 * @return A Quaternion representing this collection of Euler Angles
 * 
 * @note Returned Quaternion will reorient the coordinate axes so that the x-axis points at the given
 * right ascension and declination, then roll the coordinate axes counterclockwise (i.e., the stars
 * will appear to rotate clockwise). This is an "improper" z-y'-x' Euler rotation.
*/
inline Quaternion SphericalToQuaternion(EulerAngles angles)
    { return SphericalToQuaternion(angles.ra, angles.de, angles.roll); }


// Spherical-Vector Conversions


/**
 * Converts spherical direction to a unit vector on the unit sphere
 * 
 * @param ra The right ascension of the direction in question
 * @param de The declination of the direction in question
 * 
 * @return A 3D unit vector that represents the vector on the unit sphere
 * corresponding to this direction
*/
Vec3 SphericalToSpatial(decimal ra, decimal de);

/**
 * Converts a unit vector on the unit sphere to a spherical direction
 * 
 * @param vec The vector to convert from
 * @param ra The right ascension that will represent the right ascension
 * of vec
 * @param de The declination that will represent the declination of vec
 * 
 * @post This function's output are the parameters ra and de, and those
 * parameters are modified after this function runs.
*/
void SpatialToSpherical(const Vec3 &, decimal &ra, decimal &de);


// Angle Conversions


/**
 * Converts an angle in radians to degrees
 * 
 * @param rad The rad of the angle
 * 
 * @return The degrees of the angle
*/
constexpr decimal RadToDeg(decimal rad) {
    return rad*DECIMAL(180.0)/DECIMAL_M_PI;
}

/**
 * Converts an angle in degrees to radians
 * 
 * @param deg The degrees of the angle
 * 
 * @return The radians of the angle
*/
constexpr decimal DegToRad(decimal deg) {
    return deg/DECIMAL(180.0)*DECIMAL_M_PI;
}

/**
 * Calculates the approximate value for the
 * inverse secant of an angle
 * 
 * @param rad The angle, in radians
 * 
 * @return The arcsecant of the angle
 * 
 * @pre rad is in radians
 * 
 * @warning rad must be in radians
*/
constexpr decimal RadToArcSec(decimal rad) {
    return RadToDeg(rad) * DECIMAL(3600.0);
}

/**
 * Calculates an angle from an inverse secant value
 * 
 * @param arcSec The arcsecant value
 * 
 * @return A possible angle value, in radians, corresponding
 * to the arcsecant value arcSec
*/
constexpr decimal ArcSecToRad(decimal arcSec) {
    return DegToRad(arcSec / DECIMAL(3600.0));
}

}  // namespace found

#endif  // SRC_COMMON_SPATIAL_ATTITUDE_UTILS_HPP_
