#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "test/common/common.hpp"

#include "command-line/parser.hpp"

namespace found {

class ParserTest : public testing::Test {
 protected:
    void TearDown() override {
        optind = 2;
    }
};

TEST_F(ParserTest, TemporaryTest1RemoveAfterImplemented) {
    int argc = 1;
    const char *argv[1] = {"found"};

    ASSERT_EQ(0, main(argc, const_cast<char **>(argv)));
}

TEST_F(ParserTest, TemporaryTest2RemoveAfterImplemented) {
    int argc = 3;
    const char *argv[3] = {"found", "--png", "none.png"};

    ASSERT_EQ(0, main(argc, const_cast<char **>(argv)));
}

TEST_F(ParserTest, CalibrationParserTestExampleBaseCase) {
    int argc = 2;
    const char *argv[] = {"found", "calibration"};
    CalibrationOptions options = ParseCalibrationOptions(argc, const_cast<char **>(argv));

    // ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.lclOrientation.ra);
    // ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.lclOrientation.de);
    // ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.lclOrientation.roll);

    // ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.refOrientation.ra);
    // ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.refOrientation.de);
    // ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.refOrientation.roll);

    // ASSERT_FALSE(options.useSameOrientation);

    // ASSERT_EQ("", options.outputFile);
}

TEST_F(ParserTest, CalibrationParserTestExampleGeneral) {
    int argc = 8;
    const char *argv[] = {"found", "calibration",
        "--local-orientation", "1 2 3",
        "--reference-orientation", "3.0,-9.0,27.2",
        "--output-file", "example.found"};
    CalibrationOptions options = ParseCalibrationOptions(argc, const_cast<char **>(argv));

    // ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(1), options.lclOrientation.ra);
    // ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(2), options.lclOrientation.de);
    // ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(3), options.lclOrientation.roll);

    // ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(3.0), options.refOrientation.ra);
    // ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(-9.0), options.refOrientation.de);
    // ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(27.2), options.refOrientation.roll);

    // ASSERT_FALSE(options.useSameOrientation);

    // ASSERT_EQ("example.found", options.outputFile);
}

TEST_F(ParserTest, CalibrationParserTestExampleUseSameOriBlank) {
    int argc = 5;
    const char *argv[] = {"found", "calibration",
        "--use-same-orientation",
        "--output-file", "example.found"};
    CalibrationOptions options = ParseCalibrationOptions(argc, const_cast<char **>(argv));

    // ASSERT_TRUE(options.useSameOrientation);
}

TEST_F(ParserTest, CalibrationParserTestExample3UseSameOriTrue) {
    int argc = 6;
    const char *argv[] = {"found", "calibration",
        "--use-same-orientation", "not_false",
        "--output-file", "example.found"};
    CalibrationOptions options = ParseCalibrationOptions(argc, const_cast<char **>(argv));

    // ASSERT_TRUE(options.useSameOrientation);
}

TEST_F(ParserTest, CalibrationParserTestExampleUseSameOriFalse) {
    const char *ori_args[] = {"", "0", "false"};
    for (const char *ori_arg : ori_args) {
        int argc = 6;
        const char *argv[] = {"found", "calibration",
            "--use-same-orientation", ori_arg,
            "--output-file", "example.found"};
        CalibrationOptions options = ParseCalibrationOptions(argc, const_cast<char **>(argv));

        // ASSERT_FALSE(options.useSameOrientation);
        optind = 2;
    }
}

TEST_F(ParserTest, CalibrationParserTestExampleUseSameOriAlone) {
    int argc = 3;
    const char *argv[] = {"found", "calibration",
        "--use-same-orientation"};
    CalibrationOptions options = ParseCalibrationOptions(argc, const_cast<char **>(argv));

    // ASSERT_TRUE(options.useSameOrientation);
}

TEST_F(ParserTest, CalibrationParserTestExample6UseSameOriEquals) {
    int argc = 3;
    const char *argv[] = {"found", "calibration",
        "--use-same-orientation=true"};
    CalibrationOptions options = ParseCalibrationOptions(argc, const_cast<char **>(argv));

    // ASSERT_TRUE(options.useSameOrientation);
}

TEST_F(ParserTest, CalibrationParserTestExampleFail) {
    int argc = 4;
    const char *argv[] = {"found", "calibration",
        "--meep", "meep"};
    ASSERT_EXIT(ParseCalibrationOptions(argc, const_cast<char **>(argv)), testing::ExitedWithCode(EXIT_FAILURE), "");
}

}  // namespace found
