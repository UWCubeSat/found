#include <gtest/gtest.h>

#include "test/common/common.hpp"

#include "src/common/spatial/camera.hpp"
#include "src/common/spatial/attitude-utils.hpp"

namespace found {

///////////////////////////////////
///// TEST FIXTURE ////////////////
///////////////////////////////////

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

///////////////////////////////////
///// CALIBRATION MATRIX TESTS ////
///////////////////////////////////

/**
 * K * K^-1 should be the identity matrix for the ideal camera.
 */
TEST_F(CameraTest, IdealCalibrationMatrixInverse) {
    Mat3 product = idealCamera.GetCalibrationMatrix()
                 * idealCamera.GetInverseCalibrationMatrix();
    Mat3 identity = Mat3::Identity();

    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            EXPECT_NEAR(product(r, c), identity(r, c), 1e-9)
                << "Mismatch at (" << r << ", " << c << ")";
        }
    }
}

/**
 * K * K^-1 should be the identity matrix for the full (non-ideal) camera.
 */
TEST_F(CameraTest, FullCalibrationMatrixInverse) {
    Mat3 product = fullCamera.GetCalibrationMatrix()
                 * fullCamera.GetInverseCalibrationMatrix();
    Mat3 identity = Mat3::Identity();

    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            EXPECT_NEAR(product(r, c), identity(r, c), 1e-9)
                << "Mismatch at (" << r << ", " << c << ")";
        }
    }
}

/**
 * Verify explicit calibration matrix values for the ideal camera.
 * With focalLength = 0.01, pixelSize = 1e-5:
 *   dx = dy = 0.01 / 1e-5 = 1000
 *   K = | 1000    0   500 |
 *       |    0 1000   500 |
 *       |    0    0     1 |
 */
TEST_F(CameraTest, IdealCalibrationMatrixValues) {
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
TEST_F(CameraTest, FullCalibrationMatrixValues) {
    const Mat3 &K = fullCamera.GetCalibrationMatrix();

    EXPECT_NEAR(K(0, 0), 10000.0, 1e-6);
    EXPECT_NEAR(K(1, 1), 12500.0, 1e-6);
    EXPECT_NEAR(K(0, 2), 640.5, 1e-9);
    EXPECT_NEAR(K(1, 2), 360.5, 1e-9);
    EXPECT_NEAR(K(2, 2), 1.0, 1e-9);
}

///////////////////////////////////
// SPATIAL TO PIXEL COORD TESTS //
///////////////////////////////////

/**
 * A point on the optical axis (0, 0, z) should project to
 * the principal point (xCenter, yCenter).
 */
TEST_F(CameraTest, SpatialToPixelOnAxis) {
    Vec2 pixel = idealCamera.SpatialToPixelCoordinates(Vec3(0, 0, 1));
    EXPECT_NEAR(pixel.x(), 500.0, 1e-9);
    EXPECT_NEAR(pixel.y(), 500.0, 1e-9);
}

/**
 * On-axis projection should be independent of depth.
 */
TEST_F(CameraTest, SpatialToPixelOnAxisDifferentDepths) {
    Vec2 p1 = idealCamera.SpatialToPixelCoordinates(Vec3(0, 0, 1));
    Vec2 p2 = idealCamera.SpatialToPixelCoordinates(Vec3(0, 0, 5));
    Vec2 p3 = idealCamera.SpatialToPixelCoordinates(Vec3(0, 0, 100));

    EXPECT_NEAR(p1.x(), p2.x(), 1e-9);
    EXPECT_NEAR(p1.y(), p2.y(), 1e-9);
    EXPECT_NEAR(p2.x(), p3.x(), 1e-9);
    EXPECT_NEAR(p2.y(), p3.y(), 1e-9);
}

/**
 * Simple right-shift: (1, 0, 1) should project to
 *   pixel_x = dx * (1/1) + xCenter = 1000 + 500 = 1500
 *   pixel_y = dy * (0/1) + yCenter = 0 + 500 = 500
 */
TEST_F(CameraTest, SpatialToPixelSimpleRight) {
    Vec2 pixel = idealCamera.SpatialToPixelCoordinates(Vec3(1, 0, 1));
    EXPECT_NEAR(pixel.x(), 1500.0, 1e-9);
    EXPECT_NEAR(pixel.y(), 500.0, 1e-9);
}

/**
 * Simple down-shift: (0, 1, 1) should project to
 *   pixel_x = 0 + 500 = 500
 *   pixel_y = 1000 * 1 + 500 = 1500
 */
TEST_F(CameraTest, SpatialToPixelSimpleDown) {
    Vec2 pixel = idealCamera.SpatialToPixelCoordinates(Vec3(0, 1, 1));
    EXPECT_NEAR(pixel.x(), 500.0, 1e-9);
    EXPECT_NEAR(pixel.y(), 1500.0, 1e-9);
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

/**
 * Negative spatial x should map left of center.
 * (-0.5, 0, 1) => (1000 * -0.5 + 500, 500) = (0, 500)
 */
TEST_F(CameraTest, SpatialToPixelNegativeX) {
    Vec2 pixel = idealCamera.SpatialToPixelCoordinates(Vec3(-0.5, 0, 1));
    EXPECT_NEAR(pixel.x(), 0.0, 1e-9);
    EXPECT_NEAR(pixel.y(), 500.0, 1e-9);
}

/**
 * Full camera with offset principal point.
 * (0, 0, 1) should still project to (xCenter, yCenter) = (640.5, 360.5)
 */
TEST_F(CameraTest, FullCameraSpatialToPixelOnAxis) {
    Vec2 pixel = fullCamera.SpatialToPixelCoordinates(Vec3(0, 0, 1));
    EXPECT_NEAR(pixel.x(), 640.5, 1e-9);
    EXPECT_NEAR(pixel.y(), 360.5, 1e-9);
}

/**
 * Full camera: (5e-6, 4e-6, 0.05) shifts by exactly 1 pixel in each direction.
 *   x/z = 5e-6 / 0.05 = 1e-4, pixel_x = 10000 * 1e-4 + 640.5 = 641.5
 *   y/z = 4e-6 / 0.05 = 8e-5, pixel_y = 12500 * 8e-5 + 360.5 = 361.5
 */
TEST_F(CameraTest, FullCameraSpatialToPixelOnePixelShift) {
    Vec2 pixel = fullCamera.SpatialToPixelCoordinates(
        Vec3(5e-6, 4e-6, 0.05));
    EXPECT_NEAR(pixel.x(), 641.5, 1e-6);
    EXPECT_NEAR(pixel.y(), 361.5, 1e-6);
}

}  // namespace found
