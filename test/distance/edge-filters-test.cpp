#include <gtest/gtest.h>

#include "src/distance/edge-filters.hpp"

namespace found {

TEST(EdgeFiltersTest, NoOpEdgeFilterDoesNotModifyPoints) {
    NoOpEdgeFilter filter;

    Points original = {
        {0, 0},
        {1, 1},
        {2, 3}
    };
    Points expected = original;

    filter.Run(original);
    for (size_t i = 0; i < original.size(); ++i) {
        EXPECT_EQ(original[i].x, expected[i].x);
        EXPECT_EQ(original[i].y, expected[i].y);
    }
}

}  // namespace found
