#include <gtest/gtest.h>

#include "test/common/common.hpp"
#include "src/distance/edge.hpp"
#include "src/distance/edge_filters.hpp"
#include "src/providers/factory.hpp"  // for CreateDistancePipelineExecutor

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

// New: options-aware provider should return std::nullopt when disabled
TEST(EdgeFiltersTest, ProvideEdgeFilteringAlgorithm_OptionsDisabledReturnsNullopt) {
    DistanceOptions opts;  // defaults to enableNoOpEdgeFilter == false
    auto filtersOpt = ProvideEdgeFilteringAlgorithm(opts);
    ASSERT_FALSE(filtersOpt.has_value());
}

// New: options-aware provider should return a no-op pipeline when enabled
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

// New: factory should take the "no filters" branch when disabled
TEST(EdgeFiltersTest, Factory_CreateDistancePipelineExecutor_WithoutFilters) {
    DistanceOptions opts;
    // Provide safe image dims for camera construction; keep image null so destructor is safe
    opts.image.image = nullptr;
    opts.image.width = 64;
    opts.image.height = 64;
    opts.enableNoOpEdgeFilter = false;  // ensure disabled

    auto exec = CreateDistancePipelineExecutor(std::move(opts));
    ASSERT_NE(exec, nullptr);
    // Do not ExecutePipeline() to avoid needing a real image; just ensure construction path is covered
}

// New: factory should take the "with filters" branch when enabled
TEST(EdgeFiltersTest, Factory_CreateDistancePipelineExecutor_WithNoOpFilter) {
    DistanceOptions opts;
    opts.image.image = nullptr;
    opts.image.width = 64;
    opts.image.height = 64;
    opts.enableNoOpEdgeFilter = true;  // enable filters path

    auto exec = CreateDistancePipelineExecutor(std::move(opts));
    ASSERT_NE(exec, nullptr);
    // Do not ExecutePipeline(); construction is sufficient to cover the branch
}

}  // namespace found
