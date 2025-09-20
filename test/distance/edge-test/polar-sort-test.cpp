#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <algorithm>
#include <random>
#include <vector>

#include "test/common/common.hpp"

#include "src/common/style.hpp"

#include "src/distance/edge.hpp"
namespace found {
//////////////////////////////
////// Test Polar Sort ///////
//////////////////////////////

Points squarePointsActual = {
    {0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0},
    {0, 1},                         {4, 1},
    {0, 2},                         {4, 2},
    {0, 3},                         {4, 3},
    {0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}
};

Points squarePointsExpected = {
    {4, 3}, {4, 4}, {3, 4}, {2, 4}, {1, 4}, {0, 4},
    {0, 3}, {0, 2}, {0, 1}, {0, 0}, {1, 0}, {2, 0},
    {3, 0}, {4, 0}, {4, 1}, {4, 2}
};

struct PolarSortTestParams {
    Vec2 center;
};

class PolarSortParameterizedTestFixture : public ::testing::TestWithParam<PolarSortTestParams> {};

INSTANTIATE_TEST_SUITE_P(
    PolarSortTest,
    PolarSortParameterizedTestFixture,
    ::testing::Values(
        PolarSortTestParams{{2, 2}},
        PolarSortTestParams{{2, 1}},
        PolarSortTestParams{{1, 1}},
        PolarSortTestParams{{3, 3}}
    )
);

TEST_P(PolarSortParameterizedTestFixture, SquarePointsPolarSort) {
    auto param = GetParam();
    Points actual = squarePointsActual;

    // Shuffle the points
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(actual.begin(), actual.end(), g);

    PolarSort(actual, param.center);
    ASSERT_POINTS_POLAR_EQ(squarePointsExpected, actual);
}
}
