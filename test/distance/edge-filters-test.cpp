#include <gtest/gtest.h>

#include "test/common/common.hpp"
#include "src/distance/edge.hpp"
#include "src/distance/edge-filters.hpp"
#include "src/providers/factory.hpp"

namespace found {

TEST(EdgeFiltersTest, EdgeFilteringAlgorithms_RunWithoutCompletionThrows) {
    EdgeFilteringAlgorithms filters;
    Points pts;
    EXPECT_THROW(filters.Run(pts), std::runtime_error);
}

TEST(EdgeFiltersTest, ProvideEdgeFilteringAlgorithm_NoOpDoesNothing) {
    Points original = {
        {0, 0},
        {1, 1},
        {2, 3}
    };
    auto filters = ProvideEdgeFilteringAlgorithm();
    Points result = filters.Run(original);
    ASSERT_EQ(original.size(), result.size());
    for (size_t i = 0; i < original.size(); ++i) {
        ASSERT_EQ(original[i].x, result[i].x);
        ASSERT_EQ(original[i].y, result[i].y);
    }
}


TEST(EdgeFiltersTest, ProvideEdgeFilteringAlgorithm_OptionsDisabledReturnsNullopt) {
    DistanceOptions opts;  
    auto filtersOpt = ProvideEdgeFilteringAlgorithm(opts);
    ASSERT_FALSE(filtersOpt.has_value());
}


TEST(EdgeFiltersTest, ProvideEdgeFilteringAlgorithm_OptionsEnabledReturnsPipelineAndNoOp) {
    DistanceOptions opts;
    opts.enableNoOpEdgeFilter = true;

    auto filtersOpt = ProvideEdgeFilteringAlgorithm(opts);
    ASSERT_TRUE(filtersOpt.has_value());

    Points original = {
        {0, 0},
        {1, 1},
        {2, 3}
    };
    Points result = filtersOpt->Run(original);
    ASSERT_EQ(original.size(), result.size());
    for (size_t i = 0; i < original.size(); ++i) {
        EXPECT_EQ(original[i].x, result[i].x);
        EXPECT_EQ(original[i].y, result[i].y);
    }
}


TEST(EdgeFiltersTest, Factory_CreateDistancePipelineExecutor_WithoutFilters) {
    DistanceOptions opts;

    opts.image.image = nullptr;
    opts.image.width = 64;
    opts.image.height = 64;
    opts.enableNoOpEdgeFilter = false;

    auto exec = CreateDistancePipelineExecutor(std::move(opts));
    ASSERT_NE(exec, nullptr);
   
}


TEST(EdgeFiltersTest, Factory_CreateDistancePipelineExecutor_WithNoOpFilter) {
    DistanceOptions opts;
    opts.image.image = nullptr;
    opts.image.width = 64;
    opts.image.height = 64;
    opts.enableNoOpEdgeFilter = true;

    auto exec = CreateDistancePipelineExecutor(std::move(opts));
    ASSERT_NE(exec, nullptr);
}

TEST(EdgeFiltersTest, ProvideEdgeFilteringAlgorithm_RunWithExternalProduct) {
    Points original = {
        {0, 0},
        {1, 1},
        {2, 3}
    };
    auto filters = ProvideEdgeFilteringAlgorithm();

    Points out;
    filters.GetProduct() = &out;

    Points result = filters.Run(original);

    ASSERT_EQ(original.size(), out.size());
    ASSERT_EQ(original.size(), result.size());
    for (size_t i = 0; i < original.size(); ++i) {
        EXPECT_EQ(original[i].x, out[i].x);
        EXPECT_EQ(original[i].y, out[i].y);
        EXPECT_EQ(original[i].x, result[i].x);
        EXPECT_EQ(original[i].y, result[i].y);
    }
}

TEST(EdgeFiltersTest, ProvideEdgeFilteringAlgorithm_CompleteTwiceThrows) {
    auto filters = ProvideEdgeFilteringAlgorithm();
    NoOpEdgeFilter anotherFilter;
    EXPECT_THROW(filters.Complete(anotherFilter), std::invalid_argument);
}


}  // namespace found
