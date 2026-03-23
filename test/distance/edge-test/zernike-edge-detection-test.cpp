#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "src/distance/edge.hpp"
#include "test/common/common.hpp"
#include "test/common/mocks/distance-mocks.hpp"

namespace found {

// Tolerance for floating-point comparison
constexpr decimal TOL = 1e-5;

testing::Matcher<Vec2<>> Vec2Equal(const Vec2<> &expected) {
    return testing::Truly([expected](const Vec2<> &arg) {
        return vectorEqual(arg, expected);
    });
}

// ---- computeZernikeKernels ----
TEST(ZernikeEdgeDetectionAlgorithmTest, ComputeZernikeKernels) {
    SimpleEdgeDetectionAlgorithm initial(0, 1, DECIMAL(0));
    ZernikeEdgeDetectionAlgorithm algo(initial, 3);
    const auto kernels = algo.computeZernikeKernels();
    const std::vector<ComplexNumber> &kernelM11 = kernels.first;
    const std::vector<ComplexNumber> &kernelM20 = kernels.second;

    const double expectedM11Real[9] = {
        0,  0, 0,
        -1, 0, 1,
        0,  0, 0
    };
    const double expectedM11Imag[9] = {
        0,  1, 0,
        0,  0, 0,
        0, -1, 0
    };
    const double expectedM20[9] = {
        0, 1, 0,
        1, 0, 1,
        0, 1, 0
    };

    for (int i = 0; i < 9; i++) {
        EXPECT_DOUBLE_EQ(kernelM11[i].real, expectedM11Real[i]);
        EXPECT_DOUBLE_EQ(kernelM11[i].imag, expectedM11Imag[i]);
        EXPECT_DOUBLE_EQ(kernelM20[i].imag, 0.0);
        EXPECT_DOUBLE_EQ(kernelM20[i].real, expectedM20[i]);
    }
}

// ---- computeZernikeMoments ----
TEST(ZernikeEdgeDetectionAlgorithmTest, ComputeZernikeMoments) {
    SimpleEdgeDetectionAlgorithm initial(0, 1, DECIMAL(0));
    ZernikeEdgeDetectionAlgorithm algo(initial, 3);
    unsigned char data[9] = {2, 4, 6, 8, 10, 12, 14, 16, 18};
    Image img{3, 3, 1, data};

    const auto kernels = algo.computeZernikeKernels();
    const std::vector<ComplexNumber> &kernelM11 = kernels.first;
    const std::vector<ComplexNumber> &kernelM20 = kernels.second;
    const std::pair<ComplexNumber, ComplexNumber> moments =
        algo.computeZernikeMoments(img, Vec2<int>{1, 1}, kernelM11, kernelM20);

    EXPECT_NEAR(moments.first.real,       4.0,   TOL);
    EXPECT_NEAR(moments.first.imag,       -12.0, TOL);
    EXPECT_NEAR(moments.second.real,      40.0,  TOL);
    EXPECT_DOUBLE_EQ(moments.second.imag, 0.0);
}

// ---- extractEdgeAngle ----
TEST(ZernikeEdgeDetectionAlgorithmTest, ExtractEdgeAngle) {
    SimpleEdgeDetectionAlgorithm initial(0, 1, DECIMAL(0));
    ZernikeEdgeDetectionAlgorithm algo(initial, 3);
    const ComplexNumber A11{4.0, -12.0};

    decimal angle = algo.extractEdgeAngle(A11);

    EXPECT_NEAR(angle, -1.249045772398, TOL);
}

// ---- extractEdgeOffset ----
TEST(ZernikeEdgeDetectionAlgorithmTest, ExtractEdgeOffset) {
    SimpleEdgeDetectionAlgorithm initial(0, 1, DECIMAL(0));
    ZernikeEdgeDetectionAlgorithm algo(initial, 3);
    decimal l = algo.extractEdgeOffset(20.0, 10.0);
    EXPECT_DOUBLE_EQ(l, -0.84447279202);
}

TEST(ZernikeEdgeDetectionAlgorithmTest, ExtractEdgeOffset_ZeroA11Prime) {
    SimpleEdgeDetectionAlgorithm initial(0, 1, DECIMAL(0));
    ZernikeEdgeDetectionAlgorithm algo(initial, 3);
    decimal l = algo.extractEdgeOffset(DECIMAL(0.0), DECIMAL(0.25));
    EXPECT_DOUBLE_EQ(l, 0.0);
}

TEST(ZernikeEdgeDetectionAlgorithmTest, ExtractEdgeOffset_NegativeDiscriminant) {
    SimpleEdgeDetectionAlgorithm initial(0, 1, DECIMAL(0));
    ZernikeEdgeDetectionAlgorithm algo(initial, 3);
    decimal l = algo.extractEdgeOffset(DECIMAL(0.5), DECIMAL(-0.1));
    EXPECT_DOUBLE_EQ(l, 0.0);
}

TEST(ZernikeEdgeDetectionAlgorithmTest, ExtractEdgeOffset_LLessthanNegOne) {
    SimpleEdgeDetectionAlgorithm initial(0, 1, DECIMAL(0));
    ZernikeEdgeDetectionAlgorithm algo(initial, 3);
    decimal l = algo.extractEdgeOffset(20.0, 5.0);
    EXPECT_DOUBLE_EQ(l, -1.0);
}

TEST(ZernikeEdgeDetectionAlgorithmTest, ExtractEdgeOffset_LGreaterthanOne) {
    SimpleEdgeDetectionAlgorithm initial(0, 1, DECIMAL(0));
    ZernikeEdgeDetectionAlgorithm algo(initial, 3);
    decimal l = algo.extractEdgeOffset(20.0, -20.0);
    EXPECT_DOUBLE_EQ(l, 1.0);
}

// ---- applyEdgeCorrection ----
TEST(ZernikeEdgeDetectionAlgorithmTest, ApplyEdgeCorrection) {
    SimpleEdgeDetectionAlgorithm initial(0, 1, DECIMAL(0));
    ZernikeEdgeDetectionAlgorithm algo(initial, 3);
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

    SimpleEdgeDetectionAlgorithm initial(0, 1, DECIMAL(0));
    ZernikeEdgeDetectionAlgorithm algo(initial, 3);

    Points expected = {
        {2, 0},
        {2, 1},
        {2, 2},
        {2, 3},
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

TEST(ZernikeEdgeDetectionAlgorithmTest, Run_NegativeWindow) {
    unsigned char data[4] = {1, 2, 3, 4};
    Image img{2, 2, 1, data};
    SimpleEdgeDetectionAlgorithm initial(0, 1, DECIMAL(0));
    ZernikeEdgeDetectionAlgorithm algo(initial, -2);

    ASSERT_THROW(algo.Run(img), std::invalid_argument);
}

TEST(ZernikeEdgeDetectionAlgorithmTest, Run_InitialPointsEmpty) {
    unsigned char data[4] = {0, 0, 0, 0};
    Image img{2, 2, 1, data};
    SimpleEdgeDetectionAlgorithm initial(0, 1, DECIMAL(0));
    ZernikeEdgeDetectionAlgorithm algo(initial, 3);
    Points result = algo.Run(img);
    EXPECT_TRUE(result.empty());
}

}  // namespace found
