#include <gtest/gtest.h>

#include <memory>

#include "test/common/common.hpp"
#include "src/providers/converters.hpp"
#include "src/distance/edge.hpp"

#include <Eigen/Dense>

namespace found {

TEST(ZernikeEdgeDetectionTest, ExampleSpheroidWithInertialInitialReturnsPoints) {
    Image image = strtoimage("test/common/assets/example_spheroid2.png");

    Eigen::Matrix<decimal, Eigen::Dynamic, 1> emptyMask(0, 1);
    auto inertial = std::make_unique<InertialSymmetryEdgeDetectionAlgorithm>(
        /* grayThreshold */ 128,
        /* lineCount */ 31,
        /* lineEpsilon */ 1.0,
        emptyMask,
        /* sparseness */ DECIMAL(1.0));

    ZernikeEdgeDetectionAlgorithm zernike(std::move(inertial), /* windowSize */ 7, DECIMAL(1.66));

    Points points = zernike.Run(image);

    EXPECT_FALSE(points.empty()) << "Zernike edge detection with inertial initial should return edge points for example_spheroid2";

    stbi_image_free(image.image);
}

}  // namespace found
