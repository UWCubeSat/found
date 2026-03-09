
#include "common/spatial/attitude-utils.hpp"

#include <math.h>
#include <assert.h>

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
    return rotation.conjugate();
}

Eigen::Matrix<decimal, Eigen::Dynamic, 1> TLS(const Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> &data) {
    assert(data.cols() > 2);

    // Since the input matrix will be thin and tall, the last column of V transpose
    // will correspond to the vector with the smallest corresponding value in S,
    // meaning it is the closest vector to the null space of the input
    // We leave out Eigen::ComputeFullU so U will not be computed
    Eigen::JacobiSVD<Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic>> svd(data, Eigen::ComputeFullV);
    Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> VT = svd.matrixV();

    // rows and cols are the same size here but I clarify so that it's more readable
    Eigen::Matrix<decimal, Eigen::Dynamic, 1> finalCol = VT.col(VT.cols()-1);

    // We do head(rows - 1) because the function asks for the number numbers in the vector
    // not the index. Since we're just looking for a vector in the null space, it can have
    // arbitrary scaling. We therefore divide by the final entry (and negate) so that
    // the result dots to the output as we expect.
    // note we do *1/[] rather than /[] because division isn't defined for the vector class
    return finalCol.head(VT.rows()-1) * (-decimal(1) / finalCol(VT.rows()-1));
}

}  // namespace found
