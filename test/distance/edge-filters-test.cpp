#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>
#include <utility>

#include "src/distance/edge.hpp"
#include "src/distance/edge-filters.hpp"

#include "test/common/mocks/distance-mocks.hpp"
#include "test/common/common.hpp"

namespace found {

TEST(EdgeFiltersTest, RunOnEmptyThrows) {
    EdgeFilteringAlgorithms pipeline;
    Points input = { {0,0} };
    ASSERT_THROW(pipeline.Run(input), std::runtime_error);
}

TEST(EdgeFiltersTest, SingleMockFilterModifiesPoints) {
    EdgeFilteringAlgorithms pipeline;
    std::unique_ptr<MockEdgeFilteringAlgorithm> mockFilter = std::make_unique<MockEdgeFilteringAlgorithm>();

    EXPECT_CALL(*mockFilter, Run(testing::_))
        .WillOnce(testing::Invoke([](Points &pts) {
            pts.push_back({42, 43});
        }));

    pipeline.Complete(std::move(mockFilter));

    Points original = {
        {0, 0},
        {1, 1},
        {2, 3}
    };

    Points result = pipeline.Run(original);
    ASSERT_EQ(original.size() + 1, result.size());
    EXPECT_EQ(42, result.back().x);
    EXPECT_EQ(43, result.back().y);
}

}  // namespace found
