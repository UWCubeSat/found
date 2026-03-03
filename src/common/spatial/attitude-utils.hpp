#ifndef SRC_COMMON_SPATIAL_ATTITUDE_UTILS_HPP_
#define SRC_COMMON_SPATIAL_ATTITUDE_UTILS_HPP_

#include <Eigen/Core>
#include <Eigen/Geometry>

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


/// Attitude Conversions


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
 * @return An EulerAngle representing the equatorial coordinates of the z-axis with length 1,
 * as well as the roll about that axis, expressed in radians.
*/
EulerAngles QuaternionToSpherical(const Quaternion &quat);

/**
 * Converts Euler Angles into a quaternion
 * 
 * @param ra The right ascension of the Euler Angles
 * @param dec The declination of the Euler Angles
 * @param roll The roll of the Euler Angles
 * 
 * @return A quaternion representing the backwards rotation that will transform the vector defined
 * by the Euler angles back into the equatorial frame.
 * 
 * @note This does not necessarilycomplete the full rotation from camera coordinates into equatorial coordinates
 * since the camera coordinate defintion might have some additional rotation built in.
 */
Quaternion SphericalToQuaternion(decimal ra, decimal dec, decimal roll);

/**
 * Converts Euler Angles into a quaternion
 * 
 * @param angles The euler angles to convert
 * 
 * @return A quaternion representing the backwards rotation that will transform the vector defined
 * by the Euler angles back into the equatorial frame.
 * 
 * @note This does not necessarilycomplete the full rotation from camera coordinates into equatorial coordinates
 * since the camera coordinate defintion might have some additional rotation built in.
*/
inline Quaternion SphericalToQuaternion(EulerAngles angles)
    { return SphericalToQuaternion(angles.x(), angles.y(), angles.z()); }


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
