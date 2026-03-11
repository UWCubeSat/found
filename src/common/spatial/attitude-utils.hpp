#ifndef SRC_COMMON_SPATIAL_ATTITUDE_UTILS_HPP_
#define SRC_COMMON_SPATIAL_ATTITUDE_UTILS_HPP_

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/SVD>

#include <memory>

#include "common/decimal.hpp"

namespace found {

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

///////////////////////////////////
///////// MATRIX CLASS ////////////
///////////////////////////////////

/**
 * A Mat2 is a 2x2 Matrix
 *
 * @note Backed by Eigen::Matrix<decimal, 2, 2>.
 * Access entries via (i, j). Key methods: .col(j), .row(i), .trace(),
 * .determinant(), .transpose(), .inverse(), +, * (Mat2, Vec2, scalar),
 * Mat2::Identity()
 */
typedef Eigen::Matrix<decimal, 2, 2> Mat2;

/**
 * A Mat3 is a 3x3 Matrix
 *
 * @note Backed by Eigen::Matrix<decimal, 3, 3>.
 * Access entries via (i, j). Key methods: .col(j), .row(i), .trace(),
 * .determinant(), .transpose(), .inverse(), +, * (Mat3, Vec3, scalar),
 * Mat3::Identity()
 */
typedef Eigen::Matrix<decimal, 3, 3> Mat3;

/**
 * EulerAngles represents a 3D orientation via right ascension, declination, and roll.
 *
 * @note Backed by Eigen::Matrix<decimal, 3, 1> (Vec3).
 * Components: .x() = right ascension (ra), .y() = declination (de), .z() = roll.
 * We use z-y'-z'' Euler angles (yaw-pitch-roll order).
 */
typedef Vec3 EulerAngles;

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
typedef Eigen::Quaternion<decimal> Quaternion;

/// Convenience alias for Eigen::AngleAxis<decimal>
typedef Eigen::AngleAxis<decimal> AngleAxis;

///////////////////////////////////
////// CONVERSION FUNCTIONS ///////
///////////////////////////////////


/// Attitude Conversions

/**
 * Converts Euler Angles into a quaternion
 * 
 * @param ra The right ascension of the Euler Angles
 * @param dec The declination of the Euler Angles
 * @param roll The roll of the Euler Angles
 * 
 * @return A quaternion representing camera -> equatorial reference frame transformation 
 * that corresponds to the given Euler angles.
 * 
 * @note The x-axis (optical axis) points to the equatorial coordinates (RA, Dec) 
 * given by the Euler angles. The roll is the last rotation applied and
 * is clockwise!!! (positive) about the camera's x-axis. 
 */
Quaternion SphericalToQuaternion(decimal ra, decimal dec, decimal roll);

/**
 * Converts Euler Angles into a quaternion
 * 
 * @param angles The euler angles to convert
 * 
 * @return A quaternion representing camera -> equatorial reference frame transformation 
 * that corresponds to the given Euler angles.
 *
 * @note The x-axis (optical axis) points to the equatorial coordinates (RA, Dec) 
 * given by the Euler angles. The roll is the last rotation applied and
 * is clockwise!!! (positive) about the camera's x-axis.  
*/
inline Quaternion SphericalToQuaternion(EulerAngles angles)
    { return SphericalToQuaternion(angles.x(), angles.y(), angles.z()); }


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
Eigen::Matrix<decimal, Eigen::Dynamic, 1> TLS(const Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> &data);


/// Angle Conversions


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
