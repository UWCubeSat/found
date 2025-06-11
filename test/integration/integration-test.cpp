#include <gtest/gtest.h>

#include <sstream>
#include <string>

#include "stb_image/stb_image.h"

#include "test/common/common.hpp"

#include "command-line/parsing/parser.hpp"

namespace found {

class ParserTest : public testing::Test {
 protected:
    void TearDown() override {
        optind = 2;
    }
};

TEST_F(ParserTest, TestMainNothing) {
    int argc = 1;
    const char *argv[1] = {"found"};

    ASSERT_EQ(EXIT_FAILURE, main(argc, const_cast<char **>(argv)));
}

TEST_F(ParserTest, TestMainNoOption) {
    int argc = 3;
    const char *argv[3] = {"found", "--png", "none.png"};

    ASSERT_EQ(EXIT_FAILURE, main(argc, const_cast<char **>(argv)));
}

TEST_F(ParserTest, TestMainHelp) {
    int argc = 2;
    const char *argv[2] = {"found", "-h"};

    testing::internal::CaptureStdout();  // Start capturing stdout

    ASSERT_EQ(EXIT_SUCCESS, main(argc, const_cast<char **>(argv)));

    std::string output1 = testing::internal::GetCapturedStdout();  // Stop capturing stdout
    ASSERT_NE(static_cast<size_t>(0), output1.size());

    argv[1] = "--help";

    testing::internal::CaptureStdout();  // Start capturing stdout

    ASSERT_EQ(EXIT_SUCCESS, main(argc, const_cast<char **>(argv)));

    std::string output2 = testing::internal::GetCapturedStdout();  // Stop capturing stdout
    ASSERT_NE(static_cast<size_t>(0), output2.size());
}

// TODO: Update the next 3 tests to test once we get meaningful outputs,
// and expand to even more tests to cover a range of inputs and outputs

TEST_F(ParserTest, TestMainCalibrationOptionBlank) {
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

TEST_F(ParserTest, TestMainDistanceOption) {
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

TEST_F(ParserTest, TestMainDistanceOptionReferenceAsOrientation) {
    int argc = 5;
    const char *argv[] = {"found", "distance",
        "--image", "test/common/assets/example_image.jpg",
        "--reference-as-orientation"};

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

// TODO: Uncomment when orbit stage is implemented
// TEST_F(ParserTest, TestMainOrbitOption) {
//     int argc = 14;
//     const char *argv[] = {"found", "orbit",
//         "--position-data", "test/common/assets/position-data/pos-data-valid.txt",
//         "--output-form", "xml",
//         "--total-time", "7200.0",
//         "--mu", "168251.0"};

//     ASSERT_EQ(EXIT_SUCCESS, main(argc, const_cast<char **>(argv)));
// }

}  // namespace found
