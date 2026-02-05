#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <memory>

#include "src/compression/compression.hpp"
#include "test/common/mocks/compression-mocks.hpp"

namespace found {

TEST(CompressionAlgorithmTest, RunMissingImagePath) {
    CCSDS123CompressionAlgorithm algorithm("test/compression_output", 0, 0, 0, 0, "u8be");

    CompressionResult result = algorithm.Run("");

    ASSERT_NE(0, result.result);
}

TEST(CompressionAlgorithmTest, RunMissingOutputDir) {
    CCSDS123CompressionAlgorithm algorithm("", 0, 0, 0, 0, "u8be");

    CompressionResult result = algorithm.Run("test/common/assets/ISS034-E-54251-u8be-1x1024x1024.raw");

    ASSERT_NE(0, result.result);
}

TEST(CompressionAlgorithmTest, RunNegativeAel) {
    CCSDS123CompressionAlgorithm algorithm("test/compression_output", -1, 0, 0, 0, "u8be");

    CompressionResult result = algorithm.Run("test/common/assets/ISS034-E-54251-u8be-1x1024x1024.raw");

    ASSERT_NE(0, result.result);
}

TEST(CompressionAlgorithmTest, RunEnsureDirFailure) {
    const std::string outputPath = "test/compression_output/compression_output_file";
    std::ofstream file(outputPath);
    file << "not-a-directory";
    file.close();

    CCSDS123CompressionAlgorithm algorithm(outputPath, 0, 0, 0, 0, "u8be");

    CompressionResult result = algorithm.Run("test/common/assets/ISS034-E-54251-u8be-1x1024x1024.raw");

    ASSERT_NE(0, result.result);
}

TEST(CompressionAlgorithmTest, RunCompressionFailure) {
    CCSDS123CompressionAlgorithm algorithm("test/compression_output", 0, 0, 0, 0, "u8be");

    CompressionResult result = algorithm.Run("test/common/assets/does-not-exist.raw");

    ASSERT_NE(0, result.result);
}

TEST(CompressionPipelineTest, DoActionRunsPipeline) {
    CompressionPipeline pipeline;
    std::unique_ptr<MockCompressionAlgorithm> mockCompressionAlgorithm =
        std::make_unique<MockCompressionAlgorithm>();

    CompressionResult expected{0, "test/compression_output/mock"};
    EXPECT_CALL(*mockCompressionAlgorithm, Run("test/common/assets/ISS034-E-54251-u8be-1x1024x1024.raw"))
        .WillOnce(testing::Return(expected));

    pipeline.Complete(*mockCompressionAlgorithm);
    pipeline.GetResource() = "test/common/assets/ISS034-E-54251-u8be-1x1024x1024.raw";

    Pipeline<std::string, CompressionResult, 1> *basePipeline = &pipeline;
    basePipeline->DoAction();

    CompressionResult *product = pipeline.GetProduct();
    ASSERT_NE(nullptr, product);
    ASSERT_EQ(expected.result, product->result);
    ASSERT_EQ(expected.outputDir, product->outputDir);
}

}  // namespace found
