#include <gtest/gtest.h>

#include <stb_image/stb_image.h>

#include <sstream>
#include <string>
#include <istream>
#include <cstdio>

#include "test/common/common.hpp"

#include "src/datafile/datafile.hpp"
#include "src/datafile/serialization.hpp"

#include "src/calibrate/calibrate.hpp"

#include "src/command-line/parsing/parser.hpp"
#include "src/command-line/parsing/options.hpp"

namespace found {

/// The default arc second tolerance
#define DEFAULT_ARC_SEC_TOL DECIMAL(1500)  // Equivalent to 5/12 of a degree
/// The default magnitude error tolerance
#define DEFAULT_MAG_ERR_TOL DECIMAL(0.01)  // Equivalent to 1%

class IntegrationTest : public testing::Test {
 protected:
    void TearDown() override {
        optind = 2;
        std::remove(temp_df);
    }
};

TEST_F(IntegrationTest, TestMainNothing) {
    int argc = 1;
    const char *argv[1] = {"found"};

    ASSERT_EQ(EXIT_FAILURE, main(argc, const_cast<char **>(argv)));
}

TEST_F(IntegrationTest, TestMainNoOption) {
    int argc = 3;
    const char *argv[3] = {"found", "--png", "none.png"};

    ASSERT_EQ(EXIT_FAILURE, main(argc, const_cast<char **>(argv)));
}

TEST_F(IntegrationTest, TestMainHelp) {
    int argc = 2;
    const char *argv[2] = {"found", "-h"};

    testing::internal::CaptureStdout();  // Start capturing stdout

    ASSERT_EQ(EXIT_SUCCESS, main(argc, const_cast<char **>(argv)));

    std::string output1 = testing::internal::GetCapturedStdout();  // Stop capturing stdout
    ASSERT_NE(static_cast<size_t>(0), output1.size());

    argv[1] = "--help";
    optind = 2;

    testing::internal::CaptureStdout();  // Start capturing stdout

    ASSERT_EQ(EXIT_SUCCESS, main(argc, const_cast<char **>(argv)));

    std::string output2 = testing::internal::GetCapturedStdout();  // Stop capturing stdout
    ASSERT_NE(static_cast<size_t>(0), output2.size());
}

// TODO: Add more integration tests here to make this more complete

TEST_F(IntegrationTest, TestMainCalibrationBadDistanceAlgorithm) {
    int argc = 4;
    const char *argv[] = {"found", "distance", "--distance-algo", "NEDDA"};

    ASSERT_THROW(main(argc, const_cast<char **>(argv)), std::runtime_error);
}

TEST_F(IntegrationTest, TestMainCalibrationOptionBlank) {
    int argc = 2;
    const char *argv[] = {"found", "calibration"};

    testing::internal::CaptureStdout();  // Start capturing stdout

    ASSERT_EQ(EXIT_SUCCESS, main(argc, const_cast<char **>(argv)));

    std::string output = testing::internal::GetCapturedStdout();  // Stop capturing stdout

    std::stringstream expectedOutput;
    expectedOutput << "\\[INFO\\s[0-9]{4}-[0-9]{2}-[0-9]{2}\\s[0-9]{2}:[0-9]{2}:[0-9]{2}\\s[A-Z]+\\] "
                   << "Calibration Quaternion: \\(1, 0, 0, 0\\)\\s*";

    ASSERT_THAT(output, testing::MatchesRegex(expectedOutput.str()));
}

TEST_F(IntegrationTest, TestMainCalibrationGeneral) {
    int argc = 8;
    const char *argv[] = {"found", "calibration",
                          "--reference-orientation", "1.1,1.2,1.3",
                          "--local-orientation", "1.4,1.5,1.6",
                          "--output-file", temp_df};

    testing::internal::CaptureStdout();  // Start capturing stdout

    ASSERT_EQ(EXIT_SUCCESS, main(argc, const_cast<char **>(argv)));

    DataFile expected{
        {},
        LOSTCalibrationAlgorithm().Run({{1.1,1.2,1.3}, {1.4,1.5,1.6}})
    };

    std::string output = testing::internal::GetCapturedStdout();  // Stop capturing stdout

    std::ifstream file(temp_df);
    DataFile actual = deserializeDataFile(file);

    ASSERT_DF_EQ(expected, actual, 1);
}

TEST_F(IntegrationTest, TestMainDistanceWithManualRelOrientationPrint) {
    int argc = 6;
    const char *argv[] = {"found", "distance",
        "--image", "test/common/assets/example_image.jpg",
        "--reference-orientation", "1.1 1.2 1.3",
        "--relative-orientation", "1.4 1.5 1.6"};

    testing::internal::CaptureStdout();  // Start capturing stdout

    ASSERT_EQ(EXIT_SUCCESS, main(argc, const_cast<char **>(argv)));

    std::string output = testing::internal::GetCapturedStdout();  // Stop capturing stdout

    // Current output is just nothing, it outputs the {0, 0, 0} m vector
    std::stringstream expectedOutput;
    expectedOutput << "\\[INFO\\s[0-9]{4}-[0-9]{2}-[0-9]{2}\\s[0-9]{2}:[0-9]{2}:[0-9]{2}\\s[A-Z]+\\] "
                   << "Calculated Position: \\(-?0, -?0, -?0\\) m\\s*"
                   << "\\[INFO\\s[0-9]{4}-[0-9]{2}-[0-9]{2}\\s[0-9]{2}:[0-9]{2}:[0-9]{2}\\s[A-Z]+\\] "
                   << "Distance from Earth: 0 m\\s*";

    ASSERT_THAT(output, testing::MatchesRegex(expectedOutput.str()));
}

TEST_F(IntegrationTest, TestMainDistanceOptionReferenceAsOrientationPrint) {
    int argc = 5;
    const char *argv[] = {"found", "distance",
        "--image", "test/common/assets/example_image.jpg",
        "--reference-as-orientation"};

    testing::internal::CaptureStdout();  // Start capturing stdout

    ASSERT_EQ(EXIT_SUCCESS, main(argc, const_cast<char **>(argv)));

    std::string output = testing::internal::GetCapturedStdout();  // Stop capturing stdout

    // Current output is just nothing, it outputs the {0, 0, 0} m vector
    std::stringstream expectedOutput;
    expectedOutput << ".*\\[INFO\\s[0-9]{4}-[0-9]{2}-[0-9]{2}\\s[0-9]{2}:[0-9]{2}:[0-9]{2}\\s[A-Z]+\\] "
                   << "Calculated Position: \\(-?0, -?0, -?0\\) m\\s*"
                   << "\\[INFO\\s[0-9]{4}-[0-9]{2}-[0-9]{2}\\s[0-9]{2}:[0-9]{2}:[0-9]{2}\\s[A-Z]+\\] "
                   << "Distance from Earth: 0 m\\s*";

    ASSERT_THAT(output, testing::MatchesRegex(expectedOutput.str()));
}

TEST_F(IntegrationTest, TestIndependentDistancePipeline) {
    int argc = 13;
    const char *argv[] = {"found", "distance",
                        "--image", example_earth1.path,
                        "--reference-as-orientation",
                        "--camera-focal-length", example_earth1.FocalLength.c_str(),
                        "--camera-pixel-size", example_earth1.PixelSize.c_str(),
                        "--reference-orientation", "140,0,0",
                        "--output-file", temp_df};

    ASSERT_EQ(EXIT_SUCCESS, main(argc, const_cast<char **>(argv)));

    std::ifstream file(temp_df);
    DataFile actual = deserializeDataFile(file);

    ASSERT_EQ(static_cast<size_t>(1), actual.header.num_positions);
    ASSERT_QUAT_EQ_DEFAULT(Quaternion(1, 0, 0, 0), actual.relative_attitude);
    ASSERT_GE(DEFAULT_MAG_ERR_TOL,
              (example_earth1.position.Magnitude() - actual.positions[0].position.Magnitude())
                / example_earth1.position.Magnitude());
    ASSERT_GE(DEFAULT_ARC_SEC_TOL, RadToArcSec(Angle(example_earth1.position, actual.positions[0].position)));
}

TEST_F(IntegrationTest, TestIndependentDistancePipelineWithISDDA) {
    int argc = 21;
    const char *argv[] = {"found", "distance",
                        "--image", example_earth1.path,
                        "--reference-as-orientation",
                        "--camera-focal-length", example_earth1.FocalLength.c_str(),
                        "--camera-pixel-size", example_earth1.PixelSize.c_str(),
                        "--reference-orientation", "140,0,0",
                        "--distance-algo", ISDDA,
                        "--isdda-min-iterations", "1000",
                        "--isdda-discrim-ratio", "1.84874e+10",
                        "--isdda-dist-ratio", "0.8",
                        "--output-file", temp_df};

    ASSERT_EQ(EXIT_SUCCESS, main(argc, const_cast<char **>(argv)));

    std::ifstream file(temp_df);
    DataFile actual = deserializeDataFile(file);

    ASSERT_EQ(static_cast<size_t>(1), actual.header.num_positions);
    ASSERT_QUAT_EQ_DEFAULT(Quaternion(1, 0, 0, 0), actual.relative_attitude);
    ASSERT_GE(DEFAULT_MAG_ERR_TOL,
              (example_earth1.position.Magnitude() - actual.positions[0].position.Magnitude())
                / example_earth1.position.Magnitude());
    ASSERT_GE(DEFAULT_ARC_SEC_TOL, RadToArcSec(Angle(example_earth1.position, actual.positions[0].position)));
}

TEST_F(IntegrationTest, TestCalibrationDistanceCombinedPipeline) {
    int argc1 = 8;
    const char *argv1[] = {"found", "calibration",
                        "--reference-orientation", "20,0,0",
                        "--local-orientation", "50,0,0",
                        "--output-file", temp_df};

    ASSERT_EQ(EXIT_SUCCESS, main(argc1, const_cast<char **>(argv1)));

    optind = 2;

    int argc2 = 12;
    const char *argv2[] = {"found", "distance",
                        "--image", example_earth1.path,
                        "--calibration-data", temp_df,
                        "--camera-focal-length", example_earth1.FocalLength.c_str(),
                        "--camera-pixel-size", example_earth1.PixelSize.c_str(),
                        "--reference-orientation", "190,0,0"};
    // The relative orientation is EulerAngles{-30, 0, 0}, while the
    // reference orientation is {190,0,0}, which adds to a total
    // orientation of {140, 0, 0} (they add up in this case, but
    // in general, they do not)

    ASSERT_EQ(EXIT_SUCCESS, main(argc2, const_cast<char **>(argv2)));

    std::ifstream file(temp_df);
    DataFile actual = deserializeDataFile(file);

    ASSERT_EQ(static_cast<size_t>(1), actual.header.num_positions);
    ASSERT_QUAT_EQ(SphericalToQuaternion(example_earth1.orientation), actual.relative_attitude, 1);
    ASSERT_GE(DEFAULT_MAG_ERR_TOL,
              (example_earth1.position.Magnitude() - actual.positions[0].position.Magnitude())
                / example_earth1.position.Magnitude());
    ASSERT_GE(DEFAULT_ARC_SEC_TOL, RadToArcSec(Angle(example_earth1.position, actual.positions[0].position)));
}

TEST_F(IntegrationTest, TestCalibrationDistanceCombinedPipelineOtherOutput) {
    int argc1 = 8;
    const char *argv1[] = {"found", "calibration",
                        "--reference-orientation", "20,0,0",
                        "--local-orientation", "50,0,0",
                        "--output-file", temp_df};

    ASSERT_EQ(EXIT_SUCCESS, main(argc1, const_cast<char **>(argv1)));

    optind = 2;

    const char *other_path = "test/common/assets/other.found";

    int argc2 = 14;
    const char *argv2[] = {"found", "distance",
                        "--image", example_earth1.path,
                        "--calibration-data", temp_df,
                        "--camera-focal-length", example_earth1.FocalLength.c_str(),
                        "--camera-pixel-size", example_earth1.PixelSize.c_str(),
                        "--reference-orientation", "190,0,0",
                        "--output-file", other_path};
    // The relative orientation is EulerAngles{-30, 0, 0}, while the
    // reference orientation is {190,0,0}, which adds to a total
    // orientation of {140, 0, 0} (they add up in this case, but
    // in general, they do not)

    ASSERT_EQ(EXIT_SUCCESS, main(argc2, const_cast<char **>(argv2)));

    std::ifstream file(other_path);
    DataFile actual = deserializeDataFile(file);

    ASSERT_EQ(static_cast<size_t>(1), actual.header.num_positions);
    ASSERT_QUAT_EQ(SphericalToQuaternion(example_earth1.orientation), actual.relative_attitude, 1);
    ASSERT_GE(DEFAULT_MAG_ERR_TOL,
              (example_earth1.position.Magnitude() - actual.positions[0].position.Magnitude())
                / example_earth1.position.Magnitude());
    ASSERT_GE(DEFAULT_ARC_SEC_TOL, RadToArcSec(Angle(example_earth1.position, actual.positions[0].position)));

    std::remove(other_path);
}

// TODO: Uncomment when orbit stage is implemented
// TEST_F(IntegrationTest, TestMainOrbitOption) {
//     int argc = 14;
//     const char *argv[] = {"found", "orbit",
//         "--position-data", pos_data,
//         "--output-form", "xml",
//         "--total-time", "7200.0",
//         "--mu", "168251.0"};

//     ASSERT_EQ(EXIT_SUCCESS, main(argc, const_cast<char **>(argv)));
// }

}  // namespace found
