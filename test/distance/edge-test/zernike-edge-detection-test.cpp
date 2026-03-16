#include <gtest/gtest.h>

#include <cmath>
#include <memory>
#include <vector>

#include <Eigen/Core>
#include <Eigen/SVD>

#include "test/common/common.hpp"
#include "common/style.hpp"
#include "common/decimal.hpp"
#include "distance/edge.hpp"
#include "providers/converters.hpp"

using found::Image;
using found::Points;
using found::Vec2;
using found::SobelEdgeDetectionAlgorithm;
using found::ZernikeEdgeDetectionAlgorithm;

namespace {

// Expected conic coefficients (normalized so first is 1): a*x^2 + b*x*y + c*y^2 + d*x + e*y + f = 0
const std::vector<decimal> kExpectedConicCoeffs = {
    DECIMAL(1.0),
    DECIMAL(-0.0),
    DECIMAL(-3.8571355),
    DECIMAL(-1.024e3),
    DECIMAL(6.6482856e3),
    DECIMAL(-2.1306532e6)
};

// Fit conic to points via TLS (null space of design matrix). Returns 6 coefficients
// normalized so the first coefficient is 1.
Eigen::Matrix<decimal, 6, 1> FitConicNormalized(const Points &points) {
    const int n = static_cast<int>(points.size());
    if (n < 6) return Eigen::Matrix<decimal, 6, 1>::Zero();

    Eigen::Matrix<decimal, Eigen::Dynamic, 6> data(n, 6);
    for (int i = 0; i < n; i++) {
        decimal x = points[i].x();
        decimal y = points[i].y();
        data.row(i) << x * x, x * y, y * y, x, y, DECIMAL(1.0);
    }

    Eigen::JacobiSVD<Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic>> svd(
        data, Eigen::ComputeFullV);
    Eigen::Matrix<decimal, 6, 1> coeff = svd.matrixV().col(5);

    decimal scale = coeff(0);
    if (std::abs(static_cast<double>(scale)) < 1e-12) return Eigen::Matrix<decimal, 6, 1>::Zero();
    coeff /= scale;
    return coeff;
}

}  // namespace

TEST(ZernikeEdgeDetectionTest, SobelPlusZernikeOnSpheroidLooseConicFit) {
    Image image = found::strtoimage("test/common/assets/example_spheroid.png");

    auto sobel = std::make_unique<SobelEdgeDetectionAlgorithm>(DECIMAL(0.15));
    ZernikeEdgeDetectionAlgorithm zernike(std::move(sobel), 7, DECIMAL(1.66));

    Points edgePoints = zernike.Run(image);
    stbi_image_free(image.image);

    ASSERT_GE(edgePoints.size(), 6u) << "Need at least 6 points to fit a conic";

    Eigen::Matrix<decimal, 6, 1> fitted = FitConicNormalized(edgePoints);

    // Loose alignment check: normalized conic coefficients should be close to expected
    const decimal tol = DECIMAL(200.0);  // loose tolerance for "loosely align"
    EXPECT_NEAR(static_cast<double>(fitted(0)), 1.0, 0.01) << "Leading coefficient should be 1";
    EXPECT_NEAR(static_cast<double>(fitted(1)), static_cast<double>(kExpectedConicCoeffs[1]),
               static_cast<double>(tol));
    EXPECT_NEAR(static_cast<double>(fitted(2)), static_cast<double>(kExpectedConicCoeffs[2]),
               static_cast<double>(tol));
    EXPECT_NEAR(static_cast<double>(fitted(3)), static_cast<double>(kExpectedConicCoeffs[3]),
               static_cast<double>(tol));
    EXPECT_NEAR(static_cast<double>(fitted(4)), static_cast<double>(kExpectedConicCoeffs[4]),
               static_cast<double>(tol));
    // Constant term has large magnitude; use looser absolute tolerance
    EXPECT_NEAR(static_cast<double>(fitted(5)), static_cast<double>(kExpectedConicCoeffs[5]),
               2e5);
}
