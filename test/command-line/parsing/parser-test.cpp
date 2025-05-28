#include <gtest/gtest.h>
#include <gmock/gmock.h>

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

TEST_F(ParserTest, ParseNothing) {
    int argc = 1;
    const char *argv[1] = {"found"};

    ASSERT_EQ(1, main(argc, const_cast<char **>(argv)));
}

TEST_F(ParserTest, ParseNoOption) {
    int argc = 3;
    const char *argv[3] = {"found", "--png", "none.png"};

    // TODO: Change exepcted value from 0 to 1 after main is uncommented
    ASSERT_EQ(0, main(argc, const_cast<char **>(argv)));
}

TEST_F(ParserTest, CalibrationParserTestExampleBaseCase) {
    int argc = 2;
    const char *argv[] = {"found", "calibration"};
    CalibrationOptions options = ParseCalibrationOptions(argc, const_cast<char **>(argv));

    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.lclOrientation.ra);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.lclOrientation.de);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.lclOrientation.roll);

    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.refOrientation.ra);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.refOrientation.de);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.refOrientation.roll);

    ASSERT_FALSE(options.useSameOrientation);

    ASSERT_EQ("", options.outputFile);
}

TEST_F(ParserTest, CalibrationParserTestExampleGeneral) {
    int argc = 8;
    const char *argv[] = {"found", "calibration",
        "--local-orientation", "1 2 3",
        "--reference-orientation", "3.0,-9.0,27.2",
        "--output-file", "example.found"};
    CalibrationOptions options = ParseCalibrationOptions(argc, const_cast<char **>(argv));

    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(1), options.lclOrientation.ra);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(2), options.lclOrientation.de);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(3), options.lclOrientation.roll);

    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(3.0), options.refOrientation.ra);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(-9.0), options.refOrientation.de);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(27.2), options.refOrientation.roll);

    ASSERT_FALSE(options.useSameOrientation);

    ASSERT_EQ("example.found", options.outputFile);
}

TEST_F(ParserTest, CalibrationParserTestExampleUseSameOriBlank) {
    int argc = 5;
    const char *argv[] = {"found", "calibration",
        "--use-same-orientation",
        "--output-file", "example.found"};
    CalibrationOptions options = ParseCalibrationOptions(argc, const_cast<char **>(argv));

    ASSERT_TRUE(options.useSameOrientation);
}

TEST_F(ParserTest, CalibrationParserTestExample3UseSameOriTrue) {
    int argc = 6;
    const char *argv[] = {"found", "calibration",
        "--use-same-orientation", "not_false",
        "--output-file", "example.found"};
    CalibrationOptions options = ParseCalibrationOptions(argc, const_cast<char **>(argv));

    ASSERT_TRUE(options.useSameOrientation);
}

TEST_F(ParserTest, CalibrationParserTestExampleUseSameOriFalse) {
    const char *ori_args[] = {"", "0", "false"};
    for (const char *ori_arg : ori_args) {
        int argc = 6;
        const char *argv[] = {"found", "calibration",
            "--use-same-orientation", ori_arg,
            "--output-file", "example.found"};
        CalibrationOptions options = ParseCalibrationOptions(argc, const_cast<char **>(argv));

        ASSERT_FALSE(options.useSameOrientation);
        optind = 2;
    }
}

TEST_F(ParserTest, CalibrationParserTestExampleUseSameOriAlone) {
    int argc = 3;
    const char *argv[] = {"found", "calibration",
        "--use-same-orientation"};
    CalibrationOptions options = ParseCalibrationOptions(argc, const_cast<char **>(argv));

    ASSERT_TRUE(options.useSameOrientation);
}

TEST_F(ParserTest, CalibrationParserTestExample6UseSameOriEquals) {
    int argc = 3;
    const char *argv[] = {"found", "calibration",
        "--use-same-orientation=true"};
    CalibrationOptions options = ParseCalibrationOptions(argc, const_cast<char **>(argv));

    ASSERT_TRUE(options.useSameOrientation);
}

TEST_F(ParserTest, CalibrationParserTestExampleFail) {
    int argc = 4;
    const char *argv[] = {"found", "calibration",
        "--meep", "meep"};
    ASSERT_EXIT(ParseCalibrationOptions(argc, const_cast<char **>(argv)), testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST_F(ParserTest, DistanceParserTestBadFlag) {
    int argc = 4;
    const char *argv[] = {"found", "distance",
        "--meep", "meep"};
    ASSERT_EXIT(ParseDistanceOptions(argc, const_cast<char **>(argv)), testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST_F(ParserTest, DistanceParserTestExampleBaseCase) {
    int argc = 2;
    const char *argv[] = {"found", "distance"};
    DistanceOptions options = ParseDistanceOptions(argc, const_cast<char **>(argv));

    Image emptyImage{0, 0, 0, nullptr};
    EulerAngles emptyEA(0, 0, 0);

    ASSERT_IMAGE_EQ(emptyImage, options.image);
    ASSERT_EQ("", options.calibrationData);  // TODO: Change this once implemented
    ASSERT_FALSE(options.refAsOrientation);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0.012), options.focalLength);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(20E-6), options.pixelSize);
    ASSERT_EA_EQ_DEFAULT(emptyEA, options.refOrientation);
    ASSERT_EA_EQ_DEFAULT(emptyEA, options.relOrientation);
}

TEST_F(ParserTest, DistanceParserTestExampleGeneral) {
    int argc = 16;
    const char *argv[] = {"found", "distance",
        "--image", "test/common/assets/example_image.jpg",
        "--calibration-data", "example.found",
        "--reference-as-orientation", "false",
        "--camera-focal-length", "1.5",
        "--camera-pixel-size", "4E-12",
        "--reference-orientation", "1.1 1.2 1.3",
        "--relative-orientation", "1.4 1.5 1.6"};
    DistanceOptions options = ParseDistanceOptions(argc, const_cast<char **>(argv));
    Image expectedImage = strtoimage("test/common/assets/example_image.jpg");
    EulerAngles expectedRefOrientation(1.1, 1.2, 1.3);
    EulerAngles expectedRelOrientation(1.4, 1.5, 1.6);

    ASSERT_IMAGE_EQ(expectedImage, options.image);

    ASSERT_EQ("example.found", options.calibrationData);
    ASSERT_FALSE(options.refAsOrientation);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(1.5), options.focalLength);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(4E-12), options.pixelSize);
    ASSERT_EA_EQ_DEFAULT(expectedRefOrientation, options.refOrientation);
    ASSERT_EA_EQ_DEFAULT(expectedRelOrientation, options.relOrientation);

    stbi_image_free(expectedImage.image);  // Free the image memory
    stbi_image_free(options.image.image);  // Free the image memory
}

TEST_F(ParserTest, DistanceParserTestNoRefAsOriValue) {
    int argc = 7;
    const char *argv[] = {"found", "distance",
        "--image", "test/common/assets/example_image.jpg",
        "--reference-as-orientation",
        "--calibration-data", "example.found"};
    DistanceOptions options = ParseDistanceOptions(argc, const_cast<char **>(argv));
    Image expectedImage = strtoimage("test/common/assets/example_image.jpg");
    EulerAngles expectedRefOrientation(1.1, 1.2, 1.3);
    EulerAngles expectedRelOrientation(1.4, 1.5, 1.6);

    ASSERT_IMAGE_EQ(expectedImage, options.image);

    ASSERT_EQ("example.found", options.calibrationData);
    ASSERT_TRUE(options.refAsOrientation);

    stbi_image_free(expectedImage.image);  // Free the image memory
    stbi_image_free(options.image.image);  // Free the image memory
}

TEST_F(ParserTest, DistanceParserRefAsOriWithEquals) {
    int argc = 3;
    const char *argv[] = {"found", "distance",
        "--reference-as-orientation=true"};
    DistanceOptions options = ParseDistanceOptions(argc, const_cast<char **>(argv));

    ASSERT_TRUE(options.refAsOrientation);
}

TEST_F(ParserTest, DistanceParserRefAsOriEnd) {
    int argc = 3;
    const char *argv[] = {"found", "distance",
        "--reference-as-orientation"};
    DistanceOptions options = ParseDistanceOptions(argc, const_cast<char **>(argv));

    ASSERT_TRUE(options.refAsOrientation);
}

TEST_F(ParserTest, OrbitParserBadFlag) {
    int argc = 4;
    const char *argv[] = {"found", "orbit",
        "--meep", "meep"};
    ASSERT_EXIT(ParseOrbitOptions(argc, const_cast<char **>(argv)), testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST_F(ParserTest, OrbitParserTestExampleBaseCase) {
    int argc = 2;
    const char *argv[] = {"found", "orbit"};
    OrbitOptions options = ParseOrbitOptions(argc, const_cast<char **>(argv));
    LocationRecords emptyLocations;

    ASSERT_THAT(emptyLocations, LocationRecordsEqual(options.positionData));
    ASSERT_EQ("", options.output);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(3600.0), options.totalTime);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0.01), options.dt);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(6378.0), options.radius);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(398600.4418), options.mu);
}

TEST_F(ParserTest, OrbitParserTestExampleGeneral) {
    int argc = 14;
    const char *argv[] = {"found", "orbit",
        "--position-data", "test/common/assets/position-data/pos-data-valid.txt",
        "--output-form", "json",
        "--total-time", "7200.0",
        "--time-step", "0.02",
        "--radius", "7000.0",
        "--mu", "400000.0"};
    OrbitOptions options = ParseOrbitOptions(argc, const_cast<char **>(argv));
    LocationRecords expectedLocations = {
        {1000000, Vec3(1.0, 2.0, 3.0)},
        {2000000, Vec3(4.0, 5.0, 6.0)},
        {3000000, Vec3(7.0, 8.0, 9.0)}
    };

    ASSERT_THAT(expectedLocations, LocationRecordsEqual(options.positionData));
    ASSERT_EQ("json", options.output);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(7200.0), options.totalTime);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0.02), options.dt);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(7000.0), options.radius);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(400000.0), options.mu);
}

}  // namespace found
