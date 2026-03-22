#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cmath>
#include <memory>
#include <string>

#include "src/distance/edge.hpp"
#include "test/common/mocks/distance-mocks.hpp"

namespace found {

// Tolerance for floating-point comparison
constexpr decimal TOL = 1e-5;

// Helper: create ZernikeEdgeDetectionAlgorithm with given mask size
static ZernikeEdgeDetectionAlgorithm makeAlgo(int maskSize, decimal transitionWidth = DECIMAL(1.66)) {
    return ZernikeEdgeDetectionAlgorithm(
        std::make_unique<SimpleEdgeDetectionAlgorithm>(0, 1, DECIMAL(0)),
        maskSize,
        transitionWidth);
}

// ---- computeZernikeKernels ----
TEST(ZernikeEdgeDetectionAlgorithmTest, ComputeZernikeKernels_HardcodedValues) {
    ZernikeEdgeDetectionAlgorithm algo = makeAlgo(3);
    auto [kernelZ11, kernelZ20] = algo.computeZernikeKernels();

    const double expectedZ11Real[9] = {
        -0.04984,  0.00000,  0.04984,
        -0.33333,  0.00000,  0.33333,
        -0.04984,  0.00000,  0.04984
    };
    const double expectedZ11Imag[9] = {
        -0.04984, -0.33333, -0.04984,
        0.00000,  0.00000,  0.00000,
        0.04984,  0.33333,  0.04984
    };
    const double expectedZ20[9] = {
        0.04984,  0.11683,  0.04984,
        0.11683, -0.66667,  0.11683,
        0.04984,  0.11683,  0.04984
    };

    for (int i = 0; i < 9; i++) {
        EXPECT_NEAR(static_cast<double>(kernelZ11[i].real), expectedZ11Real[i], TOL);
        EXPECT_NEAR(static_cast<double>(kernelZ11[i].imag), expectedZ11Imag[i], TOL);
        EXPECT_DOUBLE_EQ(static_cast<double>(kernelZ20[i].imag), 0.0);
        EXPECT_NEAR(static_cast<double>(kernelZ20[i].real), expectedZ20[i], TOL);
    }
}

// ---- computeZernikeMoments ----
TEST(ZernikeEdgeDetectionAlgorithmTest, ComputeZernikeMoments_HardcodedValues) {
    ZernikeEdgeDetectionAlgorithm algo = makeAlgo(3);
    unsigned char data[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Image img{3, 3, 1, data};
    auto [kernelZ11, kernelZ20] = algo.computeZernikeKernels();
    auto [A11, A20] = algo.computeZernikeMoments(
        img, Vec2<int>{1, 1}, kernelZ11.get(), kernelZ20.get());

    EXPECT_NEAR(static_cast<double>(A11.real),  2.5981, TOL);
    EXPECT_NEAR(static_cast<double>(A11.imag),  0.8660, TOL);
    EXPECT_NEAR(static_cast<double>(A20.real),  0.0,    TOL);
    EXPECT_DOUBLE_EQ(static_cast<double>(A20.imag), 0.0);
}

// ---- extractEdgeAngle ----
TEST(ZernikeEdgeDetectionAlgorithmTest, ExtractEdgeAngle_45Degrees) {
    ZernikeEdgeDetectionAlgorithm algo = makeAlgo(3);
    ComplexNumber A11{DECIMAL(1.0), DECIMAL(1.0)};

    decimal angle = algo.extractEdgeAngle(A11);

    EXPECT_NEAR(static_cast<double>(angle), M_PI / 4.0, static_cast<double>(TOL));
}

// ---- solveEdgeDistance ----
TEST(ZernikeEdgeDetectionAlgorithmTest, SolveEdgeDistance_ZeroA11Prime_ReturnsZero) {
    ZernikeEdgeDetectionAlgorithm algo = makeAlgo(3);

    decimal l = algo.solveEdgeDistance(DECIMAL(0.0), DECIMAL(0.25), DECIMAL(1.0));

    EXPECT_DOUBLE_EQ(static_cast<double>(l), 0.0);
}

TEST(ZernikeEdgeDetectionAlgorithmTest, SolveEdgeDistance_Normal_ReturnsFinite) {
    ZernikeEdgeDetectionAlgorithm algo = makeAlgo(3);
    // Use A20 < 0 so discriminant = (wSqu-1)^2 - 2*wSqu*(A20/A11Prime) is non-negative for w=1
    decimal l = algo.solveEdgeDistance(DECIMAL(0.5), DECIMAL(-0.1), DECIMAL(1.0));

    EXPECT_TRUE(std::isfinite(static_cast<double>(l)));
}

TEST(ZernikeEdgeDetectionAlgorithmTest, SolveEdgeDistance_SmallA11Prime_ReturnsFinite) {
    ZernikeEdgeDetectionAlgorithm algo = makeAlgo(3);

    decimal l = algo.solveEdgeDistance(DECIMAL(1e-11), DECIMAL(0.25), DECIMAL(1.0));

    EXPECT_DOUBLE_EQ(static_cast<double>(l), 0.0);
}

TEST(ZernikeEdgeDetectionAlgorithmTest, SolveEdgeDistance_NegativeDiscriminant_ReturnsNonFinite) {
    ZernikeEdgeDetectionAlgorithm algo = makeAlgo(3);
    // Large A20 / A11Prime makes discriminant negative -> sqrt(discriminant) NaN
    decimal l = algo.solveEdgeDistance(DECIMAL(0.5), DECIMAL(100.0), DECIMAL(1.66));

    EXPECT_FALSE(std::isfinite(static_cast<double>(l)));
}

// ---- convertPolarToPixel ----
TEST(ZernikeEdgeDetectionAlgorithmTest, ConvertPolarToPixel_90Degrees) {
    ZernikeEdgeDetectionAlgorithm algo = makeAlgo(4);
    Vec2 center{DECIMAL(1.0), DECIMAL(1.0)};
    decimal l = DECIMAL(1.0);
    decimal psi = DECIMAL(M_PI / 2.0);

    Vec2 result = algo.convertPolarToPixel(center, l, psi);

    EXPECT_DOUBLE_EQ(static_cast<double>(result.x), 1.0);
    EXPECT_DOUBLE_EQ(static_cast<double>(result.y), 3.0);
}

TEST(ZernikeEdgeDetectionAlgorithmTest, ConvertPolarToPixel_ZeroAngle) {
    ZernikeEdgeDetectionAlgorithm algo = makeAlgo(4);
    Vec2 center{DECIMAL(0.0), DECIMAL(0.0)};
    decimal l = DECIMAL(1.0);
    decimal psi = DECIMAL(0.0);

    Vec2 result = algo.convertPolarToPixel(center, l, psi);

    EXPECT_DOUBLE_EQ(static_cast<double>(result.x), 2.0);
    EXPECT_DOUBLE_EQ(static_cast<double>(result.y), 0.0);
}

TEST(ZernikeEdgeDetectionAlgorithmTest, ConvertPolarToPixel_ZeroDistance) {
    ZernikeEdgeDetectionAlgorithm algo = makeAlgo(4);
    Vec2 center{DECIMAL(5.0), DECIMAL(7.0)};
    decimal l = DECIMAL(0.0);
    decimal psi = DECIMAL(M_PI / 4.0);

    Vec2 result = algo.convertPolarToPixel(center, l, psi);

    EXPECT_DOUBLE_EQ(static_cast<double>(result.x), 5.0);
    EXPECT_DOUBLE_EQ(static_cast<double>(result.y), 7.0);
}

// ---- Run ----
TEST(ZernikeEdgeDetectionAlgorithmTest, Run_MaskDoesNotFit_KeepsInitialPoint) {
    auto mockPtr = std::make_unique<MockEdgeDetectionAlgorithm>();
    EXPECT_CALL(*mockPtr, Run(testing::_))
        .WillRepeatedly(testing::Return(Points{Vec2{DECIMAL(0), DECIMAL(0)}}));

    unsigned char data[4] = {1, 2, 3, 4};
    Image img{2, 2, 1, data};
    ZernikeEdgeDetectionAlgorithm algo(std::move(mockPtr), 3, DECIMAL(1.66));

    Points result = algo.Run(img);

    ASSERT_EQ(result.size(), 1u);
    EXPECT_DOUBLE_EQ(static_cast<double>(result[0].x), 0.0);
    EXPECT_DOUBLE_EQ(static_cast<double>(result[0].y), 0.0);
}

TEST(ZernikeEdgeDetectionAlgorithmTest, Run_EmptyInitialPoints_ReturnsEmpty) {
    ZernikeEdgeDetectionAlgorithm algo(
        std::make_unique<SimpleEdgeDetectionAlgorithm>(255, 1, DECIMAL(0)),
        7,
        DECIMAL(1.66));
    unsigned char data[49] = {0};
    Image img{7, 7, 1, data};

    Points result = algo.Run(img);

    EXPECT_TRUE(result.empty());
}

TEST(ZernikeEdgeDetectionAlgorithmTest, Run_InvalidMaskSizeNegative_Throws) {
    ZernikeEdgeDetectionAlgorithm algo(
        std::make_unique<SimpleEdgeDetectionAlgorithm>(0, 1, DECIMAL(0)),
        -1,
        DECIMAL(1.66));
    unsigned char data[1] = {0};
    Image img{1, 1, 1, data};

    EXPECT_THROW(algo.Run(img), std::invalid_argument);
}

TEST(ZernikeEdgeDetectionAlgorithmTest, Run_RefinesInitialEdges_ReturnsNonEmpty) {
    unsigned char data[25] = {0, 0, 5, 5, 5,
                              0, 0, 5, 5, 5,
                              0, 0, 5, 5, 5,
                              0, 0, 5, 5, 5,
                              0, 0, 5, 5, 5};
    Image img{5, 5, 1, data};
    ZernikeEdgeDetectionAlgorithm algo(
        std::make_unique<SimpleEdgeDetectionAlgorithm>(2, 1, DECIMAL(0)),
        5,
        DECIMAL(1.66));

    Points result = algo.Run(img);

    EXPECT_FALSE(result.empty());
}

// Covers Run() fallback when solveEdgeDistance returns non-finite (e.g. negative discriminant).
TEST(ZernikeEdgeDetectionAlgorithmTest, Run_NonFiniteL_FallbackToInitialPoint) {
    auto mockPtr = std::make_unique<MockEdgeDetectionAlgorithm>();
    EXPECT_CALL(*mockPtr, Run(testing::_))
        .WillRepeatedly(testing::Return(Points{Vec2{DECIMAL(1.0), DECIMAL(1.0)}}));

    // Bright top row, hollow center, dark bottom: gives A20/A11prime > 0.56,
    // which makes the discriminant in solveEdgeDistance negative -> l = NaN.
    unsigned char data[9] = {255, 255, 255,
                             128,   0, 128,
                               0,   0,   0};
    Image img{3, 3, 1, data};
    ZernikeEdgeDetectionAlgorithm algo(std::move(mockPtr), 3, DECIMAL(1.66));

    Points result = algo.Run(img);

    ASSERT_FALSE(result.empty());
    EXPECT_DOUBLE_EQ(static_cast<double>(result[0].x), 1.0);
    EXPECT_DOUBLE_EQ(static_cast<double>(result[0].y), 1.0);
}


// Edge case: initial point with NaN in y. Uniform image -> finite l; refinedPoint
// then has NaN in y, so algorithm falls back to initial point
TEST(ZernikeEdgeDetectionAlgorithmTest, Run_NonFiniteRefinedPointY_FallbackToInitialPoint) {
    const decimal qNaN = static_cast<decimal>(std::nan(""));
    auto mockPtr = std::make_unique<MockEdgeDetectionAlgorithm>();
    MockEdgeDetectionAlgorithm* rawMock = mockPtr.get();
    EXPECT_CALL(*rawMock, Run(testing::_))
        .WillRepeatedly(testing::Return(Points{Vec2{DECIMAL(1.0), qNaN}}));

    unsigned char data[9] = {5, 5, 5, 5, 5, 5, 5, 5, 5};
    Image img{3, 3, 1, data};
    ZernikeEdgeDetectionAlgorithm algo(std::move(mockPtr), 3, DECIMAL(1.0));

    Points result = algo.Run(img);

    ASSERT_FALSE(result.empty());
    EXPECT_DOUBLE_EQ(static_cast<double>(result[0].x), 1.0);
    EXPECT_FALSE(std::isfinite(static_cast<double>(result[0].y)));
}

// Edge case: initial point with NaN in x. Uniform image -> finite l; refinedPoint
// then has NaN in x, so algorithm falls back to initial point
TEST(ZernikeEdgeDetectionAlgorithmTest, Run_NonFiniteRefinedPointX_FallbackToInitialPoint) {
    const decimal qNaN = static_cast<decimal>(std::nan(""));
    auto mockPtr = std::make_unique<MockEdgeDetectionAlgorithm>();
    MockEdgeDetectionAlgorithm* rawMock = mockPtr.get();
    EXPECT_CALL(*rawMock, Run(testing::_))
        .WillRepeatedly(testing::Return(Points{Vec2{qNaN, DECIMAL(1.0)}}));

    unsigned char data[9] = {5, 5, 5, 5, 5, 5, 5, 5, 5};
    Image img{3, 3, 1, data};
    ZernikeEdgeDetectionAlgorithm algo(std::move(mockPtr), 3, DECIMAL(1.0));

    Points result = algo.Run(img);

    ASSERT_FALSE(result.empty());
    EXPECT_FALSE(std::isfinite(static_cast<double>(result[0].x)));
    EXPECT_DOUBLE_EQ(static_cast<double>(result[0].y), 1.0);
}

}  // namespace found
