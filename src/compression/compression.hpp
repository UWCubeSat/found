#ifndef SRC_COMPRESSION_COMPRESSION_HPP_
#define SRC_COMPRESSION_COMPRESSION_HPP_

#include <string>

#include "command-line/parsing/options.hpp"
#include "common/style.hpp"
#include "common/pipeline/stages.hpp"

namespace found {

/**
 * The CompressionAlgorithm class houses compression algorithms
 * for CCSDS-123 compliant workflows.
 */
class CompressionAlgorithm : public FunctionStage<std::string, CompressionResult> {};

/**
 * The CCSDS123CompressionAlgorithm class houses the CCSDS-123
 * compression implementation.
 */
class CCSDS123CompressionAlgorithm : public CompressionAlgorithm {
 public:
   /// Constructs a CCSDS123CompressionAlgorithm
   CCSDS123CompressionAlgorithm(std::string outputDir,
                         int ael,
                         int x,
                         int y,
                         int z,
                         std::string dtype);
    /// Destroys this algorithm
    ~CCSDS123CompressionAlgorithm() = default;

    /**
     * Runs CCSDS-123 compression on the input options.
     *
     * @param options The compression options
     *
     * @return The compression result
     */
  CompressionResult Run(const std::string &imagePath) override;

 private:
    /* Builds the output directory path for the compression output */
  std::string BuildOutputDir(const std::string &imagePath) const;

   /// The output directory
   std::string outputDir_;
   /// Absolute error limit
   int ael_ = 0;
   /// Override X dimension
   int x_ = 0;
   /// Override Y dimension
   int y_ = 0;
   /// Override Z dimension
   int z_ = 0;
   /// Override datatype
   std::string dtype_;
};

}  // namespace found

#endif  // SRC_COMPRESSION_COMPRESSION_HPP_
