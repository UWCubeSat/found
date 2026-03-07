
#include "common/spatial/attitude-utils.hpp"

#include <math.h>
#include <assert.h>
#include "common/logging.hpp"

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

// decimal Angle(const Vec3 &vec1, const Vec3 &vec2) {
//     return AngleUnit(vec1.normalized(), vec2.normalized());
// }

// decimal AngleUnit(const Vec3 &vec1, const Vec3 &vec2) {
//     decimal dot = vec1.dot(vec2);
//     // TODO: we shouldn't need this nonsense, right? how come acos sometimes gives nan?
//     return dot >= 1 ? 0 : dot <= -1 ? DECIMAL_M_PI-DECIMAL(0.0000001) : DECIMAL_ACOS(dot);
// }

///////////////////////////////////
///////// QUATERNION HELPERS //////
///////////////////////////////////

/**
 * Converts a Quaternion to Euler Angles.
 *
 * Uses the z-y'-x'' convention matching the custom ToSpherical() implementation.
 */
EulerAngles QuaternionToSpherical(const Quaternion &q) {
    // Eigen stores (x, y, z, w) internally but accessors are .w(), .x(), .y(), .z()
    // Our convention: real=w, i=x, j=y, k=z
    decimal w = q.w(), x = q.x(), y = q.y(), z = q.z();

    decimal ra = atan2(2*(-w*z+x*y), 1-2*(y*y+z*z));
    if (ra < 0)
        ra += 2*DECIMAL_M_PI;
    decimal de = -asin(2*(-w*y-x*z));
    decimal roll = -atan2(2*(-w*x+y*z), 1-2*(x*x+y*y));
    if (roll < 0)
        roll += 2*DECIMAL_M_PI;

    return EulerAngles(ra, de, roll);
}

///////////////////////////////////
////// CONVERSION FUNCTIONS ///////
///////////////////////////////////

Mat3 QuaternionToDCM(const Quaternion &quat) {
    return quat.toRotationMatrix();
}

Quaternion DCMToQuaternion(const Mat3 &dcm) {
    return Quaternion(dcm);
}

Quaternion SphericalToQuaternion(decimal ra, decimal dec, decimal roll) {
    assert(roll >= DECIMAL(0.0) && roll <= 2*DECIMAL_M_PI);
    assert(ra >= DECIMAL(0.0) && ra <= 2*DECIMAL_M_PI);
    assert(dec >= -DECIMAL_M_PI && dec <= DECIMAL_M_PI);

    // when we are modifying the coordinate axes, the quaternion multiplication works so that the
    // rotations are applied from left to right. This is the opposite as for modifying vectors.

    // It is indeed correct that a positive rotation in our right-handed coordinate frame is in the
    // clockwise direction when looking down/through the axis of rotation. Just like the right hand
    // rule for magnetic field around a current-carrying conductor.
    Quaternion a(AngleAxis(ra, Vec3(0, 0, 1)));
    Quaternion b(AngleAxis(-dec, Vec3(0, 1, 0)));
    Quaternion c(AngleAxis(-roll, Vec3(1, 0, 0)));
    Quaternion result = (a*b*c).conjugate();
    assert(DECIMAL_ABS(result.squaredNorm() - 1) < DECIMAL(0.00001));
    return result;
}

Vec3 SphericalToSpatial(const decimal ra, const decimal de) {
    return Vec3(cos(ra)*cos(de),
                sin(ra)*cos(de),
                sin(de));
}

void SpatialToSpherical(const Vec3 &vec, decimal &ra, decimal &de) {
    ra = atan2(vec.y(), vec.x());
    if (ra < 0)
        ra += DECIMAL_M_PI*2;
    de = asin(vec.z());
}

Eigen::VectorXd TLS(Eigen::MatrixXd data) {
    assert(data.cols() > 2 && "TLS input must have at least 3 columns");

    // Since the input matrix will be thin and tall, the last column of V transpose
    // will correspond to the vector with the smallest corresponding value in S,
    // meaning it is the closest vector to the null space of the input
    // We leave out Eigen::ComputeFullU so U will not be computed
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(data, Eigen::ComputeFullV);
    Eigen::MatrixXd VT = svd.matrixV();

    // rows and cols are the same size here but I clarify so that it's more readable
    Eigen::VectorXd finalCol = VT.col(VT.cols()-1);

    // We do head(rows - 1) because the function asks for the number numbers in the vector
    // not the index. Since we're just looking for a vector in the null space, it can have
    // arbitrary scaling. We therefore divide by the final entry (and negate) so that
    // the result dots to the output as we expect.
    // note we do *1/[] rather than /[] because division isn't defined for the vector class
    return finalCol.head(VT.rows()-1) * (-1/(finalCol(VT.rows()-1)));
}

}  // namespace found
