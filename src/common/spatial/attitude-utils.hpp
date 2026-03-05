#ifndef SRC_COMMON_SPATIAL_ATTITUDE_UTILS_HPP_
#define SRC_COMMON_SPATIAL_ATTITUDE_UTILS_HPP_

#include <Eigen/Core>
#include <Eigen/Geometry>

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
 * A Vec2 is a 2D Vector
 *
 * @note Backed by Eigen::Matrix<decimal, 2, 1>.
 * Access components via .x(), .y(). Key methods: .norm(), .squaredNorm(),
 * .normalized(), .dot(), +, -, * (scalar)
 */
using Vec2 = Eigen::Matrix<decimal, 2, 1>;

/**
 * A Vec3 is a 3D Vector
 *
 * @note Backed by Eigen::Matrix<decimal, 3, 1>.
 * Access components via .x(), .y(), .z(). Key methods: .norm(), .squaredNorm(),
 * .normalized(), .dot(), .cross(), +, -, * (scalar), / (scalar)
 */
using Vec3 = Eigen::Matrix<decimal, 3, 1>;

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

// decimal Angle(const Vec3 &, const Vec3 &);

// decimal AngleUnit(const Vec3 &, const Vec3 &);

///////////////////////////////////
///////// MATRIX CLASS ////////////
///////////////////////////////////

/**
 * A Mat3 is a 3x3 Matrix
 *
 * @note Backed by Eigen::Matrix<decimal, 3, 3>.
 * Access entries via (i, j). Key methods: .col(j), .row(i), .trace(),
 * .determinant(), .transpose(), .inverse(), +, * (Mat3, Vec3, scalar),
 * Mat3::Identity()
 */
using Mat3 = Eigen::Matrix<decimal, 3, 3>;

/**
 * EulerAngles represents a 3D orientation via right ascension, declination, and roll.
 *
 * @note Backed by Eigen::Matrix<decimal, 3, 1> (Vec3).
 * Components: .x() = right ascension (ra), .y() = declination (de), .z() = roll.
 * We use z-y'-x'' Euler angles (yaw-pitch-roll order).
 */
using EulerAngles = Vec3;

///////////////////////////////////
///////// QUATERNION CLASS ////////
///////////////////////////////////

/**
 * A Quaternion represents a rotation in 3D space.
 *
 * @note Backed by Eigen::Quaternion<decimal>.
 * Access components via .w() (real), .x(), .y(), .z() (imaginary i, j, k).
 * Key methods: .conjugate(), .inverse(), .norm(), .squaredNorm(),
 * .normalized(), .toRotationMatrix(), .vec(), operator* (compose rotations),
 * q * v (rotate vector v by quaternion q).
 * Constructor: Quaternion(w, x, y, z) where w is the real/scalar part.
 * Create from axis-angle: Quaternion(Eigen::AngleAxis<decimal>(angle, axis))
 */
using Quaternion = Eigen::Quaternion<decimal>;

/// Convenience alias for Eigen::AngleAxis<decimal>
using AngleAxis = Eigen::AngleAxis<decimal>;

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
 * Converts a Quaternion to Euler Angles
 * 
 * @param quat The quaternion to convert
 * 
 * @return An EulerAngles representing the same orientation as the quaternion
*/
EulerAngles QuaternionToSpherical(const Quaternion &quat);

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
 * @note Rotating a vector with this quaternion is equivalent to a backwards rotation (rotation into
 * the frame specified by the Euler angles).
 * 
 * @warning Do not change this to return a forward quaternion unless you change conversion functions between
 * Quaternions, DCMs, and Euler Angles. In this file, all 3 systems are backwards rotations.
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
 * 
 * @warning Do not change this to return a forward quaternion unless you change conversion functions between
 * Quaternions, DCMs, and Euler Angles. In this file, all 3 systems are backwards rotations.
*/
inline Quaternion SphericalToQuaternion(EulerAngles angles)
    { return SphericalToQuaternion(angles.x(), angles.y(), angles.z()); }


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

///////////////////////////////////
///// LEAST SQUARES FUNCTIONS /////
///////////////////////////////////

/**
 * Performs total least squares on the given matrix
 * using SVD
 *   
 * @param data An NxM matrix with each row corresponding to a vector and an expected dot product output
 * 
 * i.e rows enumerated by j may contain [position[j].x, position[j].y, position[j].z, 1]
 * and the algorithm will output a Vec3 that dots with each of the positions to 1 with as little
 * error as possible
 * 
 * @note the expected output must NOT be negated, as is convention in some implementations.
 * I personally find it more intuitive this way, but if it's confusing people I can change it
 * 
 * @return The vector of length (M-1) that minimizes the error
 * Notice the vector is M-1 because it must dot with the vector formed by the M-1 entries of a row
 * to get the result in the M'th entry
*/
Eigen::VectorXd TLS(Eigen::MatrixXd data){

    // Since the input matrix will be thin and tall, the last column of V transpose 
    // will correspond to the vector with the smallest corresponding value in S, 
    // meaning it is the closest vector to the null space of the input
    // We leave out Eigen::ComputeFullU so U will not be computed
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(data, Eigen::ComputeFullV);
    Eigen::MatrixXd VT = svd.matrixV().transpose();

    // rows and cols are the same size here but I clarify so that it's more readable
    return VT.col(VT.cols()-1).head(VT.rows()-1);
}

}  // namespace found

#endif  // SRC_COMMON_SPATIAL_ATTITUDE_UTILS_HPP_
