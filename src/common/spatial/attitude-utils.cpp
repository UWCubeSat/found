
#include "common/spatial/attitude-utils.hpp"

#include <algorithm>
#include <assert.h>
#include <math.h>
#include <random>
#include <utility>

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

VecX TLS(const MatXX &data) {
    assert(data.cols() > 2);

    // Since the input matrix will be thin and tall, the last column of V transpose
    // will correspond to the vector with the smallest corresponding value in S,
    // meaning it is the closest vector to the null space of the input
    // We leave out Eigen::ComputeFullU so U will not be computed
    Eigen::JacobiSVD<MatXX> svd(data, Eigen::ComputeFullV);
    MatXX VT = svd.matrixV();

    // rows and cols are the same size here but I clarify so that it's more readable
    VecX finalCol = VT.col(VT.cols()-1);

    // We do head(rows - 1) because the function asks for the number numbers in the vector
    // not the index. Since we're just looking for a vector in the null space, it can have
    // arbitrary scaling. We therefore divide by the final entry (and negate) so that
    // the result dots to the output as we expect.
    // note we do *1/[] rather than /[] because division isn't defined for the vector class
    return finalCol.head(VT.rows()-1) * (-decimal(1) / finalCol(VT.rows()-1));
}

VecX OLS(const MatXX &data) {
    assert(data.cols() >= 2);
    const Eigen::Index m = data.cols();
    MatXX A = data.leftCols(m - 1);
    VecX b = data.col(m - 1);
    return A.householderQr().solve(b);
}

VecX WLS(const MatXX &data, const VecX &weights) {
    assert(data.cols() >= 2);
    assert(weights.size() == data.rows());
    const Eigen::Index m = data.cols();
    MatXX A = data.leftCols(m - 1);
    VecX b = data.col(m - 1);
    VecX sqrtW = weights.array().sqrt();
    MatXX Aw = sqrtW.asDiagonal() * A;
    VecX bw = (sqrtW.array() * b.array()).matrix();
    return Aw.householderQr().solve(bw);
}

VecX Ridge(const MatXX &data, decimal lambda) {
    assert(data.cols() >= 2);
    assert(lambda >= decimal(0));
    const Eigen::Index m = data.cols();
    MatXX A = data.leftCols(m - 1);
    VecX b = data.col(m - 1);
    MatXX AtA = A.transpose() * A;
    AtA.diagonal().array() += lambda;
    return AtA.ldlt().solve(A.transpose() * b);
}

VecX RANSAC(const MatXX &data, decimal residual_threshold, int max_iterations,
            Eigen::Index min_samples) {
    assert(data.cols() >= 2);
    assert(data.rows() >= 1);
    const Eigen::Index n = data.rows();
    const Eigen::Index m = data.cols();
    if (min_samples <= 0) {
        min_samples = m - 1;
    }
    min_samples = std::min(min_samples, n);
    if (n < min_samples) {
        return OLS(data);
    }
    MatXX A = data.leftCols(m - 1);
    VecX b = data.col(m - 1);

    std::random_device rd;
    std::mt19937 rng(rd());
    VecI indices(n);
    for (Eigen::Index i = 0; i < n; ++i) {
        indices(i) = i;
    }

    int best_inlier_count = -1;
    VecI best_inliers(n);
    Eigen::Index best_num_inliers = 0;

    for (int iter = 0; iter < max_iterations; ++iter) {
        for (Eigen::Index i = 0; i < n - 1; ++i) {
            std::uniform_int_distribution<Eigen::Index> dist(i, n - 1);
            Eigen::Index j = dist(rng);
            std::swap(indices(i), indices(j));
        }
        MatXX Asub(min_samples, m - 1);
        VecX bsub(min_samples);
        for (Eigen::Index i = 0; i < min_samples; ++i) {
            Asub.row(i) = A.row(indices(i));
            bsub(i) = b(indices(i));
        }
        VecX x = Asub.householderQr().solve(bsub);
        VecX residuals = (A * x - b).cwiseAbs();
        VecI inliers(n);
        Eigen::Index num_inliers = 0;
        for (Eigen::Index i = 0; i < n; ++i) {
            if (residuals(i) <= residual_threshold) {
                inliers(num_inliers++) = i;
            }
        }
        if (num_inliers > best_inlier_count) {
            best_inlier_count = static_cast<int>(num_inliers);
            best_num_inliers = num_inliers;
            best_inliers.head(num_inliers) = inliers.head(num_inliers);
        }
    }

    if (best_num_inliers < min_samples) {
        return OLS(data);
    }
    Eigen::Index n_in = best_num_inliers;
    MatXX A_in(n_in, m - 1);
    VecX b_in(n_in);
    for (Eigen::Index i = 0; i < n_in; ++i) {
        A_in.row(i) = A.row(best_inliers(i));
        b_in(i) = b(best_inliers(i));
    }
    return A_in.householderQr().solve(b_in);
}

}  // namespace found
