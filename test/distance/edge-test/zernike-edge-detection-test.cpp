#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "src/distance/edge.hpp"
#include "test/common/constants/edge-constants.hpp"
#include "test/common/common.hpp"
#include "test/common/mocks/distance-mocks.hpp"

namespace found {

// Tolerance for floating-point comparison
constexpr decimal TOL = 1e-4;

testing::Matcher<Vec2<>> Vec2Equal(const Vec2<> &expected) {
    return testing::Truly([expected](const Vec2<> &arg) {
        return vectorEqual(arg, expected);
    });
}

// ---- computeZernikeKernels ----
TEST(ZernikeEdgeDetectionAlgorithmTest, ComputeZernikeKernels) {
    ZernikeEdgeDetectionAlgorithm algo(minimalSEDA, 3);
    const auto kernels = algo.computeZernikeKernels();
    const std::unique_ptr<ComplexNumber[]> &kernelM11 = kernels.first;
    const std::unique_ptr<ComplexNumber[]> &kernelM20 = kernels.second;

    const double expectedM11Real[9] = {
        -0.1373755, 0.0000000, 0.1373755,
        -0.2839506, 0.0000000, 0.2839506,
        -0.1373755, 0.0000000, 0.1373755,
    };
    const double expectedM11Imag[9] = {
        -0.1373755, -0.2839506, -0.1373755,
        0.0000000,  0.0000000,  0.0000000,
        0.1373755,  0.2839506,  0.1373755,
    };
    const double expectedM20[9] = {
        0.0915837,  0.0030665, 0.0915837,
        0.0030665, -0.3786008, 0.0030665,
        0.0915837,  0.0030665, 0.0915837,
    };

    for (int i = 0; i < 9; i++) {
        EXPECT_NEAR(kernelM11[i].real, expectedM11Real[i], TOL);
        EXPECT_NEAR(kernelM11[i].imag, expectedM11Imag[i], TOL);
        EXPECT_DOUBLE_EQ(kernelM20[i].imag, 0.0);
        EXPECT_NEAR(kernelM20[i].real, expectedM20[i], TOL);
    }
}

// ---- computeZernikeMoments ----
TEST(ZernikeEdgeDetectionAlgorithmTest, ComputeZernikeMoments) {
    ZernikeEdgeDetectionAlgorithm algo(minimalSEDA, 3);
    unsigned char data[9] = {2, 4, 6, 8, 10, 12, 14, 16, 18};
    Image img{3, 3, 1, data};

    const auto kernels = algo.computeZernikeKernels();
    const std::unique_ptr<ComplexNumber[]> &kernelM11 = kernels.first;
    const std::unique_ptr<ComplexNumber[]> &kernelM20 = kernels.second;
    const std::pair<ComplexNumber, ComplexNumber> moments =
        algo.computeZernikeMoments(img, Vec2<int>{1, 1}, kernelM11, kernelM20);

    EXPECT_NEAR(moments.first.real,  2.2348558, TOL);
    EXPECT_NEAR(moments.first.imag,  6.7045676, TOL);
    EXPECT_NEAR(moments.second.real, 0.0004460, TOL);
    EXPECT_DOUBLE_EQ(moments.second.imag, 0.0);
}

// ---- extractEdgeAngle ----
TEST(ZernikeEdgeDetectionAlgorithmTest, ExtractEdgeAngle) {
    ZernikeEdgeDetectionAlgorithm algo(minimalSEDA, 3);
    const ComplexNumber A11{4.0, -12.0};

    decimal angle = algo.extractEdgeAngle(A11);

    EXPECT_NEAR(angle, -1.249045772398, TOL);
}

// ---- extractEdgeOffset ----
TEST(ZernikeEdgeDetectionAlgorithmTest, ExtractEdgeOffset) {
    ZernikeEdgeDetectionAlgorithm algo(minimalSEDA, 3);
    decimal l = algo.extractEdgeOffset(20.0, 10.0);
    EXPECT_NEAR(l, -0.84447279202, TOL);
}

TEST(ZernikeEdgeDetectionAlgorithmTest, ExtractEdgeOffset_ZeroA11Prime) {
    ZernikeEdgeDetectionAlgorithm algo(minimalSEDA, 3);
    decimal l = algo.extractEdgeOffset(DECIMAL(0.0), DECIMAL(0.25));
    EXPECT_DOUBLE_EQ(l, 0.0);
}

TEST(ZernikeEdgeDetectionAlgorithmTest, ExtractEdgeOffset_NegativeDiscriminant) {
    ZernikeEdgeDetectionAlgorithm algo(minimalSEDA, 3);
    decimal l = algo.extractEdgeOffset(DECIMAL(1), DECIMAL(10));
    EXPECT_NEAR(l, 0.0, TOL);
}

TEST(ZernikeEdgeDetectionAlgorithmTest, ExtractEdgeOffset_LLessthanNegOne) {
    ZernikeEdgeDetectionAlgorithm algo(minimalSEDA, 3);
    decimal l = algo.extractEdgeOffset(20.0, 5.0);
    EXPECT_DOUBLE_EQ(l, -1.0);
}

// ---- applyEdgeCorrection ----
TEST(ZernikeEdgeDetectionAlgorithmTest, ApplyEdgeCorrection) {
    ZernikeEdgeDetectionAlgorithm algo(minimalSEDA, 3);
    Vec2<int> maskCenter{1, 1};
    decimal l = DECIMAL(1.0);
    decimal psi = DECIMAL(M_PI / 2.0);

    Vec2<int> result = algo.applyEdgeCorrection(maskCenter, l, psi);

    EXPECT_DOUBLE_EQ(result.x, 1.0);
    EXPECT_DOUBLE_EQ(result.y, 2.0);
}

// ---- Run ----
TEST(ZernikeEdgeDetectionAlgorithmTest, Run) {
    unsigned char imageData[25] = {
        0, 0, 5, 5, 5,
        0, 0, 5, 5, 5,
        0, 0, 5, 5, 5,
        0, 0, 5, 5, 5,
        0, 0, 5, 5, 5
    };
    Image image = {
        5,
        5,
        1,
        imageData
    };

    ZernikeEdgeDetectionAlgorithm algo(minimalSEDA, 3);

    Points expected = {
        {2, 0},
        {1, 1},
        {1, 2},
        {1, 3},
        {2, 4}
    };

    Points actual = algo.Run(image);

    std::vector<testing::Matcher<Vec2<>>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
        [](const Vec2<> &val) {
            return Vec2Equal(val);
        });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ZernikeEdgeDetectionAlgorithmTest, RunXBoundaryConditions) {
    unsigned char imageData[25] = {
        5, 0, 0, 0, 0,
        5, 0, 0, 0, 0,
        5, 0, 0, 0, 0,
        5, 0, 0, 0, 0,
        5, 0, 0, 0, 0
    };
    Image image = {
        5,
        5,
        1,
        imageData
    };

    ZernikeEdgeDetectionAlgorithm algo(thickerBorderSEDA, 3);

    Points expected = {
        {0, 0},
        {0, 1},
        {0, 2},
        {0, 3},
        {0, 4}
    };

    Points actual = algo.Run(image);

    std::vector<testing::Matcher<Vec2<>>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
        [](const Vec2<> &val) {
            return Vec2Equal(val);
        });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ZernikeEdgeDetectionAlgorithmTest, RunXRightBoundaryCondition) {
    unsigned char imageData[25] = {
        0, 0, 0, 0, 5,
        0, 0, 0, 0, 5,
        0, 0, 0, 0, 5,
        0, 0, 0, 0, 5,
        0, 0, 0, 0, 5
    };
    Image image = {
        5,
        5,
        1,
        imageData
    };

    ZernikeEdgeDetectionAlgorithm algo(minimalSEDA, 3);

    Points expected = {
        {4, 0},
        {4, 1},
        {4, 2},
        {4, 3},
        {4, 4}
    };

    Points actual = algo.Run(image);

    std::vector<testing::Matcher<Vec2<>>> matchers;
    std::transform(expected.begin(), expected.end(), std::back_inserter(matchers),
        [](const Vec2<> &val) {
            return Vec2Equal(val);
        });

    ASSERT_THAT(actual, testing::UnorderedElementsAreArray(matchers));
}

TEST(ZernikeEdgeDetectionAlgorithmTest, Run_NegativeWindow) {
    unsigned char data[4] = {1, 2, 3, 4};
    Image img{2, 2, 1, data};
    ZernikeEdgeDetectionAlgorithm algo(minimalSEDA, -2);

    ASSERT_THROW(algo.Run(img), std::invalid_argument);
}

TEST(ZernikeEdgeDetectionAlgorithmTest, Run_EvenWindow) {
    unsigned char data[4] = {1, 2, 3, 4};
    Image img{2, 2, 1, data};
    ZernikeEdgeDetectionAlgorithm algo(minimalSEDA, 2);

    ASSERT_THROW(algo.Run(img), std::invalid_argument);
}

TEST(ZernikeEdgeDetectionAlgorithmTest, Run_minimalSEDAPointsEmpty) {
    unsigned char data[4] = {0, 0, 0, 0};
    Image img{2, 2, 1, data};
    ZernikeEdgeDetectionAlgorithm algo(minimalSEDA, 3);
    Points result = algo.Run(img);
    EXPECT_TRUE(result.empty());
}

}  // namespace found
