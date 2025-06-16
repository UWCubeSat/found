#include <gtest/gtest.h>

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

TEST_F(ParserTest, CalibrationParserBaseCase) {
    int argc = 2;
    const char *argv[] = {"found", "calibration"};
    CalibrationOptions options = ParseCalibrationOptions(argc, const_cast<char **>(argv));

    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.lclOrientation.ra);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.lclOrientation.de);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.lclOrientation.roll);

    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.refOrientation.ra);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.refOrientation.de);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0), options.refOrientation.roll);

    ASSERT_EQ("", options.outputFile);
}

TEST_F(ParserTest, TestCalibrationParserGeneral) {
    int argc = 8;
    const char *argv[] = {"found", "calibration",
        "--local-orientation", "1 2 3",
        "--reference-orientation", "3.0,-9.0,27.2",
        "--output-file", temp_df};
    CalibrationOptions options = ParseCalibrationOptions(argc, const_cast<char **>(argv));

    ASSERT_DECIMAL_EQ_DEFAULT(DegToRad(1), options.lclOrientation.ra);
    ASSERT_DECIMAL_EQ_DEFAULT(DegToRad(2), options.lclOrientation.de);
    ASSERT_DECIMAL_EQ_DEFAULT(DegToRad(3), options.lclOrientation.roll);

    ASSERT_DECIMAL_EQ_DEFAULT(DegToRad(3.0), options.refOrientation.ra);
    ASSERT_DECIMAL_EQ_DEFAULT(DegToRad(-9.0), options.refOrientation.de);
    ASSERT_DECIMAL_EQ_DEFAULT(DegToRad(27.2), options.refOrientation.roll);

    ASSERT_EQ(temp_df, options.outputFile);
}

TEST_F(ParserTest, TestCalibrationParserFail) {
    int argc = 4;
    const char *argv[] = {"found", "calibration",
        "--meep", "meep"};
    ASSERT_EXIT(ParseCalibrationOptions(argc, const_cast<char **>(argv)), testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST_F(ParserTest, TestDistanceParserBadFlag) {
    int argc = 4;
    const char *argv[] = {"found", "distance",
        "--meep", "meep"};
    ASSERT_EXIT(ParseDistanceOptions(argc, const_cast<char **>(argv)), testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST_F(ParserTest, TestDistanceParserBaseCase) {
    int argc = 4;
    const char *argv[] = {"found", "distance",
                          "--calibration-data", "test/common/assets/empty-df.found"};
    DistanceOptions options(ParseDistanceOptions(argc, const_cast<char **>(argv)));

    Image emptyImage{0, 0, 0, nullptr};
    EulerAngles emptyEA(0, 0, 0);
    DataFile expectedDataFile = strtodf("test/common/assets/empty-df.found");

    ASSERT_IMAGE_EQ(emptyImage, options.image);
    ASSERT_DF_EQ_DEFAULT(expectedDataFile, options.calibrationData);
    ASSERT_FALSE(options.refAsOrientation);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0.012), options.focalLength);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(20E-6), options.pixelSize);
    ASSERT_EA_EQ_DEFAULT(emptyEA, options.refOrientation);
    ASSERT_EA_EQ_DEFAULT(emptyEA, options.relOrientation);
}

TEST_F(ParserTest, DistanceParserGeneral) {
    int argc = 30;
    const char *argv[] = {"found", "distance",
        "--image", "test/common/assets/example_image.jpg",
        "--calibration-data", "test/common/assets/empty-df.found",
        "--reference-as-orientation", "false",
        "--camera-focal-length", "1.5",
        "--camera-pixel-size", "4E-12",
        "--reference-orientation", "1.1 1.2 1.3",
        "--relative-orientation", "1.4 1.5 1.6",
        "--planetary-radius", "1964.4",
        "--seda-threshold", "62",
        "--seda-border-len", "10",
        "--seda-offset", "9.2",
        "--distance-algo", "algo",
        "--isdda-min-iterations", "30",
        "--output-file", "example.found"};
    DistanceOptions options = ParseDistanceOptions(argc, const_cast<char **>(argv));

    Image expectedImage = strtoimage("test/common/assets/example_image.jpg");
    EulerAngles expectedRefOrientation(DegToRad(1.1), DegToRad(1.2), DegToRad(1.3));
    EulerAngles expectedRelOrientation(DegToRad(1.4), DegToRad(1.5), DegToRad(1.6));
    DataFile expectedDataFile = strtodf("test/common/assets/empty-df.found");

    ASSERT_IMAGE_EQ(expectedImage, options.image);

    ASSERT_DF_EQ_DEFAULT(expectedDataFile, options.calibrationData);
    ASSERT_FALSE(options.refAsOrientation);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(1.5), options.focalLength);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(4E-12), options.pixelSize);
    ASSERT_EA_EQ_DEFAULT(expectedRefOrientation, options.refOrientation);
    ASSERT_EA_EQ_DEFAULT(expectedRelOrientation, options.relOrientation);
    ASSERT_DECIMAL_EQ_DEFAULT(1964.4, options.radius);
    ASSERT_EQ(62, options.SEDAThreshold);
    ASSERT_EQ(10, options.SEDABorderLen);
    ASSERT_DECIMAL_EQ_DEFAULT(9.2, options.SEDAOffset);
    ASSERT_EQ("algo", options.distanceAlgo);
    ASSERT_EQ(static_cast<size_t>(30), options.ISDDAMinIters);
    ASSERT_EQ("example.found", options.outputFile);

    stbi_image_free(expectedImage.image);  // Free the image memory
    stbi_image_free(options.image.image);  // Free the image memory
}

TEST_F(ParserTest, DistanceParserNoRefAsOriValue) {
    int argc = 7;
    const char *argv[] = {"found", "distance",
        "--image", "test/common/assets/example_image.jpg",
        "--reference-as-orientation",
        "--calibration-data", "test/common/assets/empty-df.found"};
    DistanceOptions options = ParseDistanceOptions(argc, const_cast<char **>(argv));
    Image expectedImage = strtoimage("test/common/assets/example_image.jpg");
    EulerAngles expectedRefOrientation(1.1, 1.2, 1.3);
    EulerAngles expectedRelOrientation(1.4, 1.5, 1.6);
    DataFile expectedDataFile = strtodf("test/common/assets/empty-df.found");

    ASSERT_IMAGE_EQ(expectedImage, options.image);

    ASSERT_DF_EQ_DEFAULT(expectedDataFile, options.calibrationData);
    ASSERT_TRUE(options.refAsOrientation);

    stbi_image_free(expectedImage.image);  // Free the image memory
    stbi_image_free(options.image.image);  // Free the image memory
}

TEST_F(ParserTest, TestDistanceParserRefAsOriWithEquals) {
    int argc = 3;
    const char *argv[] = {"found", "distance",
        "--reference-as-orientation=true"};
    DistanceOptions options = ParseDistanceOptions(argc, const_cast<char **>(argv));

    ASSERT_TRUE(options.refAsOrientation);
}

TEST_F(ParserTest, TestDistanceParserRefAsOriEnd) {
    int argc = 3;
    const char *argv[] = {"found", "distance",
        "--reference-as-orientation"};
    DistanceOptions options = ParseDistanceOptions(argc, const_cast<char **>(argv));

    ASSERT_TRUE(options.refAsOrientation);
}

TEST_F(ParserTest, TestOrbitParserBadFlag) {
    int argc = 4;
    const char *argv[] = {"found", "orbit",
        "--meep", "meep"};
    ASSERT_EXIT(ParseOrbitOptions(argc, const_cast<char **>(argv)), testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST_F(ParserTest, OrbitParserBaseCase) {
    int argc = 2;
    const char *argv[] = {"found", "orbit"};
    OrbitOptions options = ParseOrbitOptions(argc, const_cast<char **>(argv));
    LocationRecords emptyLocations;

    ASSERT_THAT(emptyLocations, LocationRecordsEqual(options.positionData));
    ASSERT_EQ("", options.output);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(3600.0), options.totalTime);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(0.01), options.dt);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL_M_R_E, options.radius);
    ASSERT_DECIMAL_EQ_DEFAULT(DECIMAL(398600.4418), options.mu);
}

TEST_F(ParserTest, OrbitParserGeneral) {
    int argc = 14;
    const char *argv[] = {"found", "orbit",
        "--position-data", pos_data,
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
