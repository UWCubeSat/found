#include <gtest/gtest.h>

#include "test/common/common.hpp"
#include "src/distance/edge.hpp"
#include "src/distance/edge_filters.hpp"

namespace found {

TEST(EdgeFiltersTest, ProvideEdgeFilteringAlgorithm_NoOpDoesNothing) {
    Points original = {
        {0, 0},
        {1, 1},
        {2, 3}
    };
    // Get the filter pipeline (contains a NoOpEdgeFilter)
    auto filters = ProvideEdgeFilteringAlgorithm();
    // Run the pipeline - ModifyingPipeline<Points>::Run returns a Points
    Points result = filters.Run(original);
    ASSERT_EQ(original.size(), result.size());
    for (size_t i = 0; i < original.size(); ++i) {
        ASSERT_EQ(original[i].x, result[i].x);
        ASSERT_EQ(original[i].y, result[i].y);
    }
}

}
