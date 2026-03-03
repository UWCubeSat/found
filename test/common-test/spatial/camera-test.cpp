#include <gtest/gtest.h>

#include "test/common/common.hpp"

#include "src/common/spatial/camera.hpp"
#include "src/common/spatial/attitude-utils.hpp"

namespace found {

// Tolerance for matrix-level identity checks (K * K^-1 == I)
#ifdef FOUND_FLOAT_MODE
    static constexpr decimal kMatrixInverseTol = 1e-4;
#else
    static constexpr decimal kMatrixInverseTol = 1e-9;
#endif

////////////////////
/// TEST FIXTURE ///
////////////////////

class CameraTest : public testing::Test {
 protected:
    // Ideal camera: focalLength = 0.01 m, pixelSize = 1e-5 m,
    // 1000x1000 resolution => dx = dy = 1000, center = (500, 500)
    Camera idealCamera{0.01, 1e-5, 1000, 1000};

    // Non-ideal camera with offset principal point
    // focalLength = 0.05 m, 1280x720, center = (640.5, 360.5),
    // xPixelPitch = 5e-6 m, yPixelPitch = 4e-6 m
    Camera fullCamera{0.05, 1280, 720, 640.5, 360.5, 5e-6, 4e-6};
};

////////////////////////////////
/// CALIBRATION MATRIX TESTS ///
////////////////////////////////

/**
 * Verify explicit calibration matrix values for the ideal camera.
 * With focalLength = 0.01, pixelSize = 1e-5:
 *   dx = dy = 0.01 / 1e-5 = 1000
 *   K = | 1000    0   500 |
 *       |    0 1000   500 |
 *       |    0    0     1 |
 */
TEST_F(CameraTest, IdealCameraCalibrationMatrixValues) {
    const Mat3 &K = idealCamera.GetCalibrationMatrix();

    EXPECT_NEAR(K(0, 0), 1000.0, 1e-9);
    EXPECT_NEAR(K(1, 1), 1000.0, 1e-9);
    EXPECT_NEAR(K(0, 2), 500.0, 1e-9);
    EXPECT_NEAR(K(1, 2), 500.0, 1e-9);
    EXPECT_NEAR(K(2, 2), 1.0, 1e-9);

    // Off-diagonal / unused entries should be zero
    EXPECT_NEAR(K(0, 1), 0.0, 1e-9);
    EXPECT_NEAR(K(1, 0), 0.0, 1e-9);
    EXPECT_NEAR(K(2, 0), 0.0, 1e-9);
    EXPECT_NEAR(K(2, 1), 0.0, 1e-9);
}

/**
 * Verify explicit calibration matrix values for the full camera.
 * focalLength = 0.05, xPixelPitch = 5e-6, yPixelPitch = 4e-6
 *   dx = 0.05 / 5e-6 = 10000
 *   dy = 0.05 / 4e-6 = 12500
 *   K = | 10000      0   640.5 |
 *       |     0  12500   360.5 |
 *       |     0      0       1 |
 */
TEST_F(CameraTest, FullCameraCalibrationMatrixValues) {
    const Mat3 &K = fullCamera.GetCalibrationMatrix();

    EXPECT_NEAR(K(0, 0), 10000.0, 1e-6);
    EXPECT_NEAR(K(1, 1), 12500.0, 1e-6);
    EXPECT_NEAR(K(0, 2), 640.5, 1e-9);
    EXPECT_NEAR(K(1, 2), 360.5, 1e-9);
    EXPECT_NEAR(K(2, 2), 1.0, 1e-9);

    // Off-diagonal / unused entries should be zero
    EXPECT_NEAR(K(0, 1), 0.0, 1e-9);
    EXPECT_NEAR(K(1, 0), 0.0, 1e-9);
    EXPECT_NEAR(K(2, 0), 0.0, 1e-9);
    EXPECT_NEAR(K(2, 1), 0.0, 1e-9);
}

/**
 * K * K^-1 should be the identity matrix for the ideal camera.
 */
TEST_F(CameraTest, IdealInverseCameraCalibrationMatrixTest) {
    Mat3 product = idealCamera.GetCalibrationMatrix()
                 * idealCamera.GetInverseCalibrationMatrix();
    Mat3 identity = Mat3::Identity();

    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            EXPECT_NEAR(product(r, c), identity(r, c), kMatrixInverseTol)
                << "Mismatch at (" << r << ", " << c << ")";
        }
    }
}

/**
 * K * K^-1 should be the identity matrix for the full (non-ideal) camera.
 */
TEST_F(CameraTest, FullInverseCameraCalibrationMatrixTest) {
    Mat3 product = fullCamera.GetCalibrationMatrix()
                 * fullCamera.GetInverseCalibrationMatrix();
    Mat3 identity = Mat3::Identity();

    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            EXPECT_NEAR(product(r, c), identity(r, c), kMatrixInverseTol)
                << "Mismatch at (" << r << ", " << c << ")";
        }
    }
}

////////////////////////////////////
/// SPATIAL TO PIXEL COORD TESTS ///
////////////////////////////////////

/**
 * On-axis projection should be independent of depth.
 */
TEST_F(CameraTest, SpatialToPixelOnAxisTest) {
    Vec2 p1 = idealCamera.SpatialToPixelCoordinates(Vec3(0, 0, 1));
    Vec2 p2 = idealCamera.SpatialToPixelCoordinates(Vec3(0, 0, 5));
    Vec2 p3 = idealCamera.SpatialToPixelCoordinates(Vec3(0, 0, 100));

    EXPECT_NEAR(p1.x(), 500.0, 1e-9);
    EXPECT_NEAR(p1.y(), 500.0, 1e-9);
    EXPECT_NEAR(p2.x(), 500.0, 1e-9);
    EXPECT_NEAR(p2.y(), 500.0, 1e-9);
    EXPECT_NEAR(p3.x(), 500.0, 1e-9);
    EXPECT_NEAR(p3.y(), 500.0, 1e-9);
}

/**
 * Simple right-shift: (1, 0, 1) should project to
 *   pixel_x = dx * (1/2) + xCenter = 500 + 500 = 1000
 *   pixel_y = dy * (0/2) + yCenter = 0 + 500 = 500
 */
TEST_F(CameraTest, SpatialToPixelSimpleRight) {
    Vec2 pixel = idealCamera.SpatialToPixelCoordinates(Vec3(1, 0, 2));
    EXPECT_NEAR(pixel.x(), 1000.0, 1e-9);
    EXPECT_NEAR(pixel.y(), 500.0, 1e-9);
}

/**
 * Simple down-shift: (0, 1, 1) should project to
 *   pixel_x = 0 + 500 = 500
 *   pixel_y = 1000 * 0.5 + 500 = 1000
 */
TEST_F(CameraTest, SpatialToPixelSimpleDown) {
    Vec2 pixel = idealCamera.SpatialToPixelCoordinates(Vec3(0, 1, 2));
    EXPECT_NEAR(pixel.x(), 500.0, 1e-9);
    EXPECT_NEAR(pixel.y(), 1000.0, 1e-9);
}

/**
 * Perspective scaling: doubling z halves the offset from center.
 * (1, 1, 2) => pixel = (dx * 0.5 + 500, dy * 0.5 + 500) = (1000, 1000)
 */
TEST_F(CameraTest, SpatialToPixelPerspective) {
    Vec2 pixel = idealCamera.SpatialToPixelCoordinates(Vec3(1, 1, 2));
    EXPECT_NEAR(pixel.x(), 1000.0, 1e-9);
    EXPECT_NEAR(pixel.y(), 1000.0, 1e-9);
}

TEST_F(CameraTest, PixelToImageCoordinatesCenterTest) {
    Vec3 imageCoords = idealCamera.PixelToImageCoordinates(Vec2(500, 500));

    EXPECT_NEAR(imageCoords.x(), 0.0, 1e-9);
    EXPECT_NEAR(imageCoords.y(), 0.0, 1e-9);
    EXPECT_NEAR(imageCoords.z(), 1.0, 1e-9);
}

TEST_F(CameraTest, PixelToImageCoordinatesOffCenterPositiveTest) {
    Vec3 imageCoords = idealCamera.PixelToImageCoordinates(Vec2(1000, 1000));

    EXPECT_NEAR(imageCoords.x(), .5, 1e-9);
    EXPECT_NEAR(imageCoords.y(), .5, 1e-9);
    EXPECT_NEAR(imageCoords.z(), 1.0, 1e-9);
}

TEST_F(CameraTest, PixelToImageCoordinatesOffCenterNegativeTest) {
    Vec3 imageCoords = idealCamera.PixelToImageCoordinates(Vec2(0, 0));

    EXPECT_NEAR(imageCoords.x(), -.5, 1e-9);
    EXPECT_NEAR(imageCoords.y(), -.5, 1e-9);
    EXPECT_NEAR(imageCoords.z(), 1.0, 1e-9);
}

////////////////////////////
/// InSensor BOUNDARY TESTS
////////////////////////////

TEST_F(CameraTest, InSensorOutOfBoundsNegativeX) {
    EXPECT_FALSE(idealCamera.InSensor(Vec2(-1, 500)));
}

TEST_F(CameraTest, InSensorOutOfBoundsLargeX) {
    EXPECT_FALSE(idealCamera.InSensor(Vec2(1001, 500)));
}

TEST_F(CameraTest, InSensorOutOfBoundsNegativeY) {
    EXPECT_FALSE(idealCamera.InSensor(Vec2(500, -1)));
}

TEST_F(CameraTest, InSensorOutOfBoundsLargeY) {
    EXPECT_FALSE(idealCamera.InSensor(Vec2(500, 1001)));
}

}  // namespace found
