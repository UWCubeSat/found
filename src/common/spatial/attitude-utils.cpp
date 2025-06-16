
#include "common/spatial/attitude-utils.hpp"

#include <math.h>
#include <assert.h>

namespace found {

///////////////////////////////////
///////// QUATERNION CLASS ////////
///////////////////////////////////

/// Multiply two quaternions using the usual definition of quaternion multiplication (effectively composes rotations)
Quaternion Quaternion::operator*(const Quaternion &other) const {
    return Quaternion(
        real*other.real - i*other.i - j*other.j - k*other.k,
        real*other.i + other.real*i + j*other.k - k*other.j,
        real*other.j + other.real*j + k*other.i - i*other.k,
        real*other.k + other.real*k + i*other.j - j*other.i);
}

/// Negate the quaternion, which is the same rotation
Quaternion Quaternion::operator-() const {
    return Quaternion(-real, -i, -j, -k);
}

/// Effectively computes a quaternion representing the inverse rotation of the original.
Quaternion Quaternion::Conjugate() const {
    return Quaternion(real, -i, -j, -k);
}

/// The vector formed by imaginary components of the quaternion. The axis of the represented rotation.
Vec3 Quaternion::Vector() const {
    return { i, j, k };
}

/**
 * Set imaginary components of Quarterion.
 * 
 * @param vec The vector holding the imaginary quantities
 * */
void Quaternion::SetVector(const Vec3 &vec) {
    i = vec.x;
    j = vec.y;
    k = vec.z;
}

/**
 * Creates a "pure quaternion" with the given vector for imaginary parts and zero for real part.
 * 
 * @param input The vector representing the imaginary part of the Quaternion to create
 * */
Quaternion::Quaternion(const Vec3 &input) {
    real = 0;
    SetVector(input);
}

/**
 * Create a quaternion which represents a rotation of theta around the axis input
 * 
 * @param input the axis about which theta is defined
 * @param theta the rotation about the input
*/
Quaternion::Quaternion(const Vec3 &input, decimal theta) {
    real = cos(theta/2);
    // the compiler will optimize it. Right?
    i = input.x * sin(theta/2);
    j = input.y * sin(theta/2);
    k = input.z * sin(theta/2);
}

/**
 * Rotates a vector about this Quaternion
 * 
 * @param input The vector to rotate about this
 * 
 * @return The input vector that has been rotated about this
*/
Vec3 Quaternion::Rotate(const Vec3 &input) const {
    // TODO: optimize
    return ((*this)*Quaternion(input)*Conjugate()).Vector();
}

/**
 * Provides the amount of rotations represented by this Quaterion
 * 
 * @return The amount of rotations represented by this, in radians
*/
decimal Quaternion::Angle() const {
    if (real <= -1) {
        return 0;  // 180*2=360=0
    }
    // TODO: we shouldn't need this nonsense, right? how come acos sometimes gives nan? (same as in AngleUnit)
    return (real >= 1 ? 0 : acos(real))*2;
}

/**
 * Changes the rotation represented in this quarterion
 * 
 * @param newAngle The rotation angle this new quarternion should represent
 * 
 * @post Changes this by altering the rotation angle of this by newAngle
*/
void Quaternion::SetAngle(decimal newAngle) {
    real = cos(newAngle/2);
    SetVector(Vector().Normalize() * sin(newAngle/2));
}

/**
 * Tells client if this Quarternion is a unit quaternion
 * 
 * @return true iff this is a unit Quaternion
 * 
*/
bool Quaternion::IsUnit(decimal tolerance) const {
    return abs(i*i+j*j+k*k+real*real - 1) < tolerance;
}

/**
 * Provides the canonicalized Quaternion of this, if it is not already
 * canonicalized
 * 
 * @return The canonicalized Quaternion of this, which is this iff this
 * is a canonicalized Quaternion already, and a new Quaternion if it is
 * not
 */
Quaternion Quaternion::Canonicalize() const {
    if (real >= 0) {
        return *this;
    }

    return Quaternion(-real, -i, -j, -k);
}

///////////////////////////////////
////// CONVERSION FUNCTIONS ///////
///////////////////////////////////

/**
 * Converts this Quarterion to a new system of Euler Angles by extracting the
 * rotations described by this quarterion
 * 
 * @return An EulerAngles object representing this in with Euler Angles
*/
EulerAngles Quaternion::ToSpherical() const {
    // Working out these equations would be a pain in the ass. Thankfully, this wikipedia page:
    // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_angles_conversion
    // uses almost exactly the same euler angle scheme that we do, so we copy their equations almost
    // wholesale. The only differences are that 1, we use de and roll in the opposite directions,
    // and 2, we store the conjugate of the quaternion (double check why?), which means we need to
    // invert the final de and roll terms, as well as negate all the terms involving a mix between
    // the real and imaginary parts.
    decimal ra = atan2(2*(-real*k+i*j), 1-2*(j*j+k*k));
    if (ra < 0)
        ra += 2*M_PI;
    decimal de = -asin(2*(-real*j-i*k));  // allow de to be positive or negaive, as is convention
    decimal roll = -atan2(2*(-real*i+j*k), 1-2*(i*i+j*j));
    if (roll < 0)
        roll += 2*M_PI;

    return EulerAngles(ra, de, roll);
}

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
Quaternion SphericalToQuaternion(decimal ra, decimal dec, decimal roll) {
    assert(roll >= 0.0 && roll <= 2*M_PI);
    assert(ra >= 0 && ra <= 2*M_PI);
    assert(dec >= -M_PI && dec <= M_PI);

    // when we are modifying the coordinate axes, the quaternion multiplication works so that the
    // rotations are applied from left to right. This is the opposite as for modifying vectors.

    // It is indeed correct that a positive rotation in our right-handed coordinate frame is in the
    // clockwise direction when looking down/through the axis of rotation. Just like the right hand
    // rule for magnetic field around a current-carrying conductor.
    Quaternion a = Quaternion({ 0, 0, 1 }, ra);
    Quaternion b = Quaternion({ 0, 1, 0 }, -dec);
    Quaternion c = Quaternion({ 1, 0, 0 }, -roll);
    Quaternion result = (a*b*c).Conjugate();
    assert(result.IsUnit(0.00001));
    return result;
}

/**
 * Converts spherical direction to a unit vector on the unit sphere
 * 
 * @param ra The right ascension of the direction in question
 * @param de The declination of the direction in question
 * 
 * @return A 3D unit vector that represents the vector on the unit sphere
 * corresponding to this direction
*/
Vec3 SphericalToSpatial(const decimal ra, const decimal de) {
    return {
        cos(ra)*cos(de),
        sin(ra)*cos(de),
        sin(de),
    };
}

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
void SpatialToSpherical(const Vec3 &vec, decimal &ra, decimal &de) {
    ra = atan2(vec.y, vec.x);
    if (ra < 0)
        ra += M_PI*2;
    de = asin(vec.z);
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
decimal RadToArcSec(decimal rad) {
    return RadToDeg(rad) * 3600.0;
}


/**
 * Calculates an angle from an inverse secant value
 * 
 * @param arcSec The arcsecant value
 * 
 * @return A possible angle value, in radians, corresponding
 * to the arcsecant value arcSec
*/
decimal ArcSecToRad(decimal arcSec) {
    return DegToRad(arcSec / 3600.0);
}

///////////////////////////////////
///////// VECTOR CLASSES //////////
///////////////////////////////////

/**
 * Provides the square of the magnitude of this Vec3
 * 
 * @return The square of the magnitude of this
*/
decimal Vec3::MagnitudeSq() const {
    return x*x+y*y+z*z;
}

/**
 * Provides the square of the magnitude of this Vec2
 * 
 * @return The square of the magnitude of this
*/
decimal Vec2::MagnitudeSq() const {
    return x*x+y*y;
}

/**
 * Provides the magnitude of this Vec3
 * 
 * @return The magnitude of this
*/
decimal Vec3::Magnitude() const {
    return sqrt(MagnitudeSq());
}

/**
 * Provides the magnitude of this Vec2
 * 
 * @return The magnitude of this
*/
decimal Vec2::Magnitude() const {
    return sqrt(MagnitudeSq());
}

/**
 * Normalizes this
 * 
 * @return The normalized vector
 * of this
 */
Vec2 Vec2::Normalize() const {
    decimal mag = Magnitude();
    return {
        x/mag, y/mag,
    };
}

/**
 * Provides the magnitude of this Vec2
 * 
 * @return The magnitude of this
*/
Vec3 Vec3::Normalize() const {
    decimal mag = Magnitude();
    return {
        x/mag, y/mag, z/mag,
    };
}

/// Dot product (Scalar product)
decimal Vec3::operator*(const Vec3 &other) const {
    return x*other.x + y*other.y + z*other.z;
}

/// Dot product (Scalar product)
decimal Vec2::operator*(const Vec2 &other) const {
    return x*other.x + y*other.y;
}

/// Scalar-vector Product
Vec2 Vec2::operator*(const decimal &other) const {
    return { x*other, y*other };
}

/// Vector-Scalar Multiplication
Vec3 Vec3::operator*(const decimal &other) const {
    return { x*other, y*other, z*other };
}

/// Vector Addition
Vec2 Vec2::operator+(const Vec2 &other) const {
    return {x + other.x, y + other.y };
}

/// Vector Subtraction
Vec2 Vec2::operator-(const Vec2 &other) const {
    return { x - other.x, y - other.y };
}

/// Vector Subtraction
Vec3 Vec3::operator-(const Vec3 &other) const {
    return { x - other.x, y - other.y, z - other.z };
}

/// Vector Negation
Vec3 Vec3::operator-() const {
    return { -x, -y, -z };
}

/// Vector Addition
Vec3 &Vec3::operator+=(const Vec3 &other) {
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;

    return *this;
}

/// Vector division
Vec3 Vec3::operator/(const decimal &divisor) const {
    return { x / divisor, y / divisor, z / divisor };
}

/**
 * Computes the cross (vector) product between this and another vector
 * 
 * @param other The vector to cross with this
 * 
 * @return A vector that is the cross product between
 * this and other
*/
Vec3 Vec3::CrossProduct(const Vec3 &other) const {
    return {
        y*other.z - z*other.y,
        -(x*other.z - z*other.x),
        x*other.y - y*other.x,
    };
}

/**
 * Computes the outer product between this and another vector
 * 
 * @param other The other vector in this operation
 * 
 * @return A matrix that is the outer product between this
 * and other
*/
Mat3 Vec3::OuterProduct(const Vec3 &other) const {
    return {
        x*other.x, x*other.y, x*other.z,
        y*other.x, y*other.y, y*other.z,
        z*other.x, z*other.y, z*other.z
    };
}

/**
 * Computes the product of a 3x3 matrix and a 3x1 vector (this)
 * 
 * @param other The matrix to multiply this with
 * 
 * @return The 3x1 vector resulting from the multiplication of
 * other and this
 * 
*/
Vec3 Vec3::operator*(const Mat3 &other) const {
    return {
        x*other.At(0,0) + y*other.At(0,1) + z*other.At(0,2),
        x*other.At(1,0) + y*other.At(1,1) + z*other.At(1,2),
        x*other.At(2,0) + y*other.At(2,1) + z*other.At(2,2),
    };
}

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
Vec2 midpoint(const Vec2 &vec1, const Vec2 &vec2) {
    return {(vec1.x + vec2.x)/2, (vec1.y + vec2.y)/2};
}

/**
 * Finds the midpoint between two different vectors
 *
 * @param vec1 The first vector
 * @param vec2 The second vector
 *
 * @return The midpoint vector
*/
Vec3 midpoint(const Vec3 &vec1, const Vec3 &vec2) {
    return {(vec1.x + vec2.x)/2, (vec1.y + vec2.y)/2, (vec1.z + vec2.z)/2};
}

/**
 * Finds the midpoint between three different vectors
 *
 * @param vec1 The first vector
 * @param vec2 The second vector
 * @param vec3 The third vector
 *
 * @return The midpoint vector
*/
Vec3 midpoint(const Vec3 &vec1, const Vec3 &vec2, const Vec3 &vec3) {
    return {(vec1.x + vec2.x + vec3.x)/3, (vec1.y + vec2.y + vec3.y)/3, (vec1.z + vec2.z + vec3.z)/3};
}


/**
 * Determines the angle between two different vectors
 * 
 * @param vec1 The first vector
 * @param vec2 The second vector
 * 
 * @return The angle, in radians, between vec1 and vec2
*/
decimal Angle(const Vec3 &vec1, const Vec3 &vec2) {
    return AngleUnit(vec1.Normalize(), vec2.Normalize());
}

/**
 * Determines the angle between two different vectors
 * 
 * @param vec1 The first vector
 * @param vec2 The second vector
 * 
 * @return The angle, in radians, between vec1 and vec2
 * 
 * @pre The magnitude of vec1 and vec2 are 1
 * 
 * @warning vec1 and vec2 must have a magnitude of 1
*/
decimal AngleUnit(const Vec3 &vec1, const Vec3 &vec2) {
    decimal dot = vec1*vec2;
    // TODO: we shouldn't need this nonsense, right? how come acos sometimes gives nan?
    return dot >= 1 ? 0 : dot <= -1 ? M_PI-0.0000001 : acos(dot);
}

/**
 * Determines the Distance between two vectors
 * 
 * @param v1 The first vector
 * @param v2 The second vector
 * 
 * @return The distance between v1 and v2
*/
decimal Distance(const Vec2 &v1, const Vec2 &v2) {
    return (v1-v2).Magnitude();
}

/**
 * Determines the Distance between two vectors
 * 
 * @param v1 The first vector
 * @param v2 The second vector
 * 
 * @return The distance between v1 and v2
*/
decimal Distance(const Vec3 &v1, const Vec3 &v2) {
    return (v1-v2).Magnitude();
}

///////////////////////////////////
///////// MATRIX CLASS ////////////
///////////////////////////////////

/**
 * Obtains an entry in this Matrix
 * 
 * @param i The row of the entry
 * @param j The column of the entry
 * 
 * @return The value of the entry in this at (i, j)
*/
decimal Mat3::At(int i, int j) const {
    return x[3*i+j];
}

/**
 * Obtains one of the column vectors in this Matrix
 * 
 * @param j The column of the vector
 * 
 * @return The vector at column j
*/
Vec3 Mat3::Column(int j) const {
    return {At(0,j), At(1,j), At(2,j)};
}

/**
 * Obtains one of the row vectors in this Matrix
 * 
 * @param i The row of the vector
 * 
 * @return The vector at row i
*/
Vec3 Mat3::Row(int i) const {
    return {At(i,0), At(i,1), At(i,2)};
}

/// Matrix Addition
Mat3 Mat3::operator+(const Mat3 &other) const {
    return {
        At(0,0)+other.At(0,0), At(0,1)+other.At(0,1), At(0,2)+other.At(0,2),
        At(1,0)+other.At(1,0), At(1,1)+other.At(1,1), At(1,2)+other.At(1,2),
        At(2,0)+other.At(2,0), At(2,1)+other.At(2,1), At(2,2)+other.At(2,2)
    };
}

/// Matrix Multiplication
Mat3 Mat3::operator*(const Mat3 &other) const {
#define _MATMUL_ENTRY(row, col) At(row,0)*other.At(0,col) + At(row,1)*other.At(1,col) + At(row,2)*other.At(2,col)
    return {
        _MATMUL_ENTRY(0,0), _MATMUL_ENTRY(0,1), _MATMUL_ENTRY(0,2),
        _MATMUL_ENTRY(1,0), _MATMUL_ENTRY(1,1), _MATMUL_ENTRY(1,2),
        _MATMUL_ENTRY(2,0), _MATMUL_ENTRY(2,1), _MATMUL_ENTRY(2,2),
    };
#undef _MATMUL_ENTRY
}

/// Matrix-Vector Multiplication (Same as Vector::operator*(const Mat3 &))
Vec3 Mat3::operator*(const Vec3 &vec) const {
    return {
        vec.x*At(0,0) + vec.y*At(0,1) + vec.z*At(0,2),
        vec.x*At(1,0) + vec.y*At(1,1) + vec.z*At(1,2),
        vec.x*At(2,0) + vec.y*At(2,1) + vec.z*At(2,2),
    };
}

/// Matrix-Scalar Multiplication
Mat3 Mat3::operator*(const decimal &s) const {
    return {
        s*At(0,0), s*At(0,1), s*At(0,2),
        s*At(1,0), s*At(1,1), s*At(1,2),
        s*At(2,0), s*At(2,1), s*At(2,2)
    };
}

/**
 * Obtains the transpose of this Matrix
 * 
 * @return The transpose Matrix of this
*/
Mat3 Mat3::Transpose() const {
    return {
        At(0,0), At(1,0), At(2,0),
        At(0,1), At(1,1), At(2,1),
        At(0,2), At(1,2), At(2,2),
    };
}

/**
 * Obtains the trace of this Matrix
 * 
 * @return The trace of this
*/
decimal Mat3::Trace() const {
    return At(0,0) + At(1,1) + At(2,2);
}

/**
 * Obtains the determinant of this Matrix
 * 
 * @return The determinant of this
*/
decimal Mat3::Det() const {
    return (At(0,0) * (At(1,1)*At(2,2) - At(2,1)*At(1,2))) -
    (At(0,1) * (At(1,0)*At(2,2) - At(2,0)*At(1,2))) +
    (At(0,2) * (At(1,0)*At(2,1) - At(2,0)*At(1,1)));
}

/**
 * Obtains the inverse of this Matrix
 * 
 * @return The inverse Matrix of this 
*/
Mat3 Mat3::Inverse() const {
    // https://ardoris.wordpress.com/2008/07/18/general-formula-for-the-inverse-of-a-3x3-matrix/
    decimal scalar = 1 / Det();

    Mat3 res = {
        At(1,1)*At(2,2) - At(1,2)*At(2,1), At(0,2)*At(2,1) - At(0,1)*At(2,2), At(0,1)*At(1,2) - At(0,2)*At(1,1),
        At(1,2)*At(2,0) - At(1,0)*At(2,2), At(0,0)*At(2,2) - At(0,2)*At(2,0), At(0,2)*At(1,0) - At(0,0)*At(1,2),
        At(1,0)*At(2,1) - At(1,1)*At(2,0), At(0,1)*At(2,0) - At(0,0)*At(2,1), At(0,0)*At(1,1) - At(0,1)*At(1,0)
    };

    return res * scalar;
}

/// 3x3 identity matrix
const Mat3 kIdentityMat3 = {1,0,0,
                            0,1,0,
                            0,0,1};


///////////////////////////////////
///////// ATTITUDE CLASS //////////
///////////////////////////////////

/**
 * Constructs an Attitude object from Quaternion information
 * 
 * @param quat The quaternion to base the attitude off of  
*/
Attitude::Attitude(const Quaternion &quat) : quaternion(quat), type(QuaternionType) {}

/**
 * Constructs an Attitude object from a Direction Cosine Matrix (A
 * matrix holding the direction cosines for an attitude)
 * 
 * @param matrix The matrix holding the direction cosines
*/
Attitude::Attitude(const Mat3 &matrix) : dcm(matrix), type(DCMType) {}

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
Mat3 QuaternionToDCM(const Quaternion &quat) {
    Vec3 x = quat.Rotate({1, 0, 0});
    Vec3 y = quat.Rotate({0, 1, 0});
    Vec3 z = quat.Rotate({0, 0, 1});
    return {
        x.x, y.x, z.x,
        x.y, y.y, z.y,
        x.z, y.z, z.z,
    };
}

/**
 * Creates a Quaternion based on a Direction Cosine Matrix (rotation matrix)
 * 
 * @param dcm The matrix holding the direction cosines
 * 
 * @return A Quaternion that expresses the rotation defined in dcm
*/
Quaternion DCMToQuaternion(const Mat3 &dcm) {
    // Make a quaternion that rotates the reference frame X-axis into the dcm's X-axis, just like
    // the DCM itself does
    Vec3 oldXAxis = Vec3({1, 0, 0});
    Vec3 newXAxis = dcm.Column(0);  // this is where oldXAxis is mapped to
    assert(abs(newXAxis.Magnitude()-1) < 0.001);
    Vec3 xAlignAxis = oldXAxis.CrossProduct(newXAxis).Normalize();
    decimal xAlignAngle = AngleUnit(oldXAxis, newXAxis);
    Quaternion xAlign(xAlignAxis, xAlignAngle);

    // Make a quaternion that will rotate the Y-axis into place
    Vec3 oldYAxis = xAlign.Rotate({0, 1, 0});
    Vec3 newYAxis = dcm.Column(1);
    // we still need to take the cross product, because acos returns a value in [0,pi], and thus we
    // need to know which direction to rotate before we rotate. We do this by checking if the cross
    // product of old and new y axes is in the same direction as the new X axis.
    bool rotateClockwise = oldYAxis.CrossProduct(newYAxis) * newXAxis > 0;  // * is dot product
    Quaternion yAlign({1, 0, 0}, AngleUnit(oldYAxis, newYAxis) * (rotateClockwise ? 1 : -1));

    // We're done! There's no need to worry about the Z-axis because the handed-ness of the
    // coordinate system is always preserved, which means the Z-axis is uniquely determined as the
    // cross product of the X- and Y-axes. This goes to show that DCMs store redundant information.

    // we want the y alignment to have memory of X, which means we put its multiplication on the
    // right
    return xAlign*yAlign;
}

/**
 * Provides the Quaternion corresponding to this Attitude
 * 
 * @return A Quaternion that holds the attitude information
 * of this
*/
Quaternion Attitude::GetQuaternion() const {
    switch (type) {
        case QuaternionType:
            return quaternion;
        case DCMType:
            return DCMToQuaternion(dcm);
        default:
            assert(false);
    }
}

/**
 * Obtains the rotation matrix from this Attitude
 * 
 * @return A matrix containing the direction cosines
 * indicated by this
*/
Mat3 Attitude::GetDCM() const {
    switch (type) {
        case DCMType:
            return dcm;
        case QuaternionType:
            return QuaternionToDCM(quaternion);
        default:
            assert(false);
    }
}

/**
 * Obtains a vector rotated to this Attitude from another vector 
 * (Convert a vector from the reference frame to the body frame.)
 * 
 * @param vec The vector to rotate
 * 
 * @return A new vector that is rotated from vec based on this
*/
Vec3 Attitude::Rotate(const Vec3 &vec) const {
    switch (type) {
        case DCMType:
            return dcm*vec;
        case QuaternionType:
            return quaternion.Rotate(vec);
        default:
            assert(false);
    }
}

/**
 * Obtains the Euler Angles of this Attitude
 * 
 * @return An EulerAngles object that holds the Euler
 * Angles of this
*/
EulerAngles Attitude::ToSpherical() const {
    switch (type) {
        case DCMType:
            return GetQuaternion().ToSpherical();
        case QuaternionType:
            return quaternion.ToSpherical();
        default:
            assert(false);
    }
}

///////////////////////////////////
///////// BUFFER OPERATIONS ///////
///////////////////////////////////

/**
 * Computes the size, in bytes, that a Vec3 object will take up
 * 
 * @return The number of bytes that a Vec3 occupies
*/
int64_t SerializeLengthVec3() {
    return sizeof(decimal)*3;
}

/**
 * Serializes a Vec3 into a buffer.
 * 
 * @param vec The vector to serialize.
 * @param buffer The buffer to insert the vector into
 * 
 * @post The parameter buffer will hold vec after the 
 * operation of this function.
 * 
 * @note A buffer is a very long character array that holds information
 * that the user defines. Serialization of data means inputting certain
 * data into a buffer.
*/
void SerializeVec3(const Vec3 &vec, unsigned char *buffer) {
    decimal *fBuffer = reinterpret_cast<decimal *>(buffer);
    *fBuffer++ = vec.x;
    *fBuffer++ = vec.y;
    *fBuffer = vec.z;
}


/**
 * Deserializes a Vec3 from a buffer
 * 
 * @param buffer The buffer to obtain the vector from. This parameter
 * should point to the location of the Vec3 within the buffer.
 * 
 * @return A Vec3 representing the Vec3 stored in the buffer
 * 
 * @pre buffer points to a valid location storing a Vec3
 * 
 * @warning Returns nonsense if buffer does not point to a valid location
 * that stores a Vec3
*/
Vec3 DeserializeVec3(const unsigned char *buffer) {
    Vec3 result;
    const decimal *fBuffer = reinterpret_cast<decimal *>(const_cast<unsigned char *>(buffer));
    result.x = *fBuffer++;
    result.y = *fBuffer++;
    result.z = *fBuffer;
    return result;
}

}  // namespace found
