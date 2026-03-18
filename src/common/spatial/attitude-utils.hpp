#ifndef SRC_COMMON_SPATIAL_ATTITUDE_UTILS_HPP_
#define SRC_COMMON_SPATIAL_ATTITUDE_UTILS_HPP_

#include <Eigen/Cholesky>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/QR>
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
typedef Eigen::Matrix<decimal, 2, 1> Vec2;

/**
 * A Vec3 is a 3D Vector
 *
 * @note Backed by Eigen::Matrix<decimal, 3, 1>.
 * Access components via .x(), .y(), .z(). Key methods: .norm(), .squaredNorm(),
 * .normalized(), .dot(), .cross(), +, -, * (scalar), / (scalar)
 */
typedef Eigen::Matrix<decimal, 3, 1> Vec3;

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
typedef Eigen::Matrix<decimal, 3, 3> Mat3;

/** Dynamic-size column vector (Nx1). */
typedef Eigen::Matrix<decimal, Eigen::Dynamic, 1> VecX;

/** Dynamic-size matrix (NxM). */
typedef Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> MatXX;

/** Dynamic-size index vector (e.g. row/column indices). */
typedef Eigen::Matrix<Eigen::Index, Eigen::Dynamic, 1> VecI;

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

/** Convenience alias for Eigen::AngleAxis<decimal> */
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
 * @return A quaternion representing equatorial->camera reference frame transformation 
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
 * @return A quaternion representing equatorial->camera reference frame transformation 
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
VecX TLS(const MatXX &data);

/**
 * Ordinary least squares: minimizes ||A*x - b||^2 over x.
 * Same data layout as TLS: rows are [position[j].x, position[j].y, ..., 1] with the last column
 * the target; returns the (M-1)-vector x such that A*x ≈ b.
 *
 * @param data NxM matrix; columns 0..M-2 are the design matrix A, column M-1 is b
 * @return The vector of length (M-1) that minimizes ||A*x - b||^2
 */
VecX OLS(const MatXX &data);

/**
 * Weighted least squares: minimizes sum_j w_j * (row_j · x - target_j)^2.
 * Same data layout as TLS; weights are per-row (observation weights).
 *
 * @param data NxM matrix; columns 0..M-2 are the design matrix, column M-1 is the target
 * @param weights N-vector of non-negative weights (one per row)
 * @return The vector of length (M-1) that minimizes the weighted residual sum of squares
 */
VecX WLS(const MatXX &data, const VecX &weights);

/**
 * Ridge regression: minimizes ||A*x - b||^2 + lambda*||x||^2.
 * Same data layout as TLS; lambda is the L2 regularization strength.
 *
 * @param data NxM matrix; columns 0..M-2 are the design matrix, column M-1 is the target
 * @param lambda Non-negative regularization parameter
 * @return The vector of length (M-1) that minimizes the ridge objective
 */
VecX Ridge(const MatXX &data, decimal lambda);

/**
 * RANSAC regression: fit a linear model by repeatedly sampling a minimal set,
 * fitting OLS on the sample, and choosing the model with the most inliers.
 * Final model is OLS refit on all inliers of the best sample.
 * Same data layout as TLS; residual_threshold is the max absolute residual for an inlier.
 *
 * @param data NxM matrix; columns 0..M-2 are the design matrix, column M-1 is the target
 * @param residual_threshold Max absolute residual for a row to count as an inlier
 * @param max_iterations Number of RANSAC trials
 * @param min_samples Minimum number of rows to fit per trial (default 0 = use M-1 for unique OLS)
 * @return The vector of length (M-1) from OLS on the inliers of the best trial
 */
VecX RANSAC(const MatXX &data, decimal residual_threshold, int max_iterations,
            Eigen::Index min_samples = 0);


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
