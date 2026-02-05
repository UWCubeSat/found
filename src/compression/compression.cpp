#include "compression/compression.hpp"

#include <array>
#include <string>
#include <utility>

#include "common/logging.hpp"
#include "command-line/parsing/parser.hpp"

extern "C" {
#include "ccsds123_internal.h"  // NOLINT(build/include_subdir)
#include "ccsds123_io.h"  // NOLINT(build/include_subdir)
}

namespace found {

///// CCSDS123CompressionAlgorithm /////

CCSDS123CompressionAlgorithm::CCSDS123CompressionAlgorithm(std::string outputDir,
                                                           int ael,
                                                           int x,
                                                           int y,
                                                           int z,
                                                           std::string dtype)
    : outputDir_(std::move(outputDir)),
      ael_(ael),
      x_(x),
      y_(y),
      z_(z),
      dtype_(std::move(dtype)) {}

CompressionResult CCSDS123CompressionAlgorithm::Run(const std::string &imagePath) {
    CompressionResult result{};

    if (imagePath.empty() || outputDir_.empty()) {
        LOG_ERROR("Compression requires --image-path and --output-dir. " << HELP_MSG);
        result.result = 1;
        return result;
    }

    if (ael_ < 0) {
        LOG_ERROR("AEL must be >= 0.");
        result.result = 1;
        return result;
    }

    if (ccsds123_ensure_dir(outputDir_.c_str()) != 0) {
        LOG_ERROR("Failed to create output dir: " << outputDir_);
        result.result = 1;
        return result;
    }

    result.result = ccsds123_compress_one_image(imagePath.c_str(),
                                                outputDir_.c_str(),
                                                ael_,
                                                x_,
                                                y_,
                                                z_,
                                                dtype_.c_str());

    if (result.result != 0) {
        LOG_ERROR("Compression failed for: " << imagePath);
        return result;
    }

    result.outputDir = BuildOutputDir(imagePath);
    return result;
}

std::string CCSDS123CompressionAlgorithm::BuildOutputDir(const std::string &imagePath) const {
    std::array<char, CCSDS123_MAX_PATH_LEN> out_dir{};
    ccsds123_build_output_folder_path(outputDir_.c_str(),
                                      imagePath.c_str(),
                                      ael_,
                                      out_dir.data());
    return std::string(out_dir.data());
}

}  // namespace found
