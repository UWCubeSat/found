#ifndef SRC_COMMON_SPATIAL_CAMERA_HPP_
#define SRC_COMMON_SPATIAL_CAMERA_HPP_

#include "common/spatial/attitude-utils.hpp"

#include "common/style.hpp"

namespace found {

///////////////////////////////////
////////// CAMERA CLASS ///////////
///////////////////////////////////

/**
 * A Camera is a mutable object that represents a pinhole camera.
 * 
 * @pre Camera dimensions are in SI
 * 
 * @note Follows the convention as laid out in Fundamentals of Space OpNav (Christian, 2026)
 * - The optical center (hole in pinhole camera model) is located at the origin (0, 0, 0)
 * - The x-axis points to the right
 * - The y-axis points down 
 * - The z-axis points along the camera's optical axis (depth)
 * - The virtual image plane is located at z = 1
 * - The detector plane is located at z = -focalLength
*/
class Camera {
 public:
    /**
     * Copy Constructor for Camera
     */
    Camera(const Camera &) = default;

    /**
     * Creates a Camera object off of real Camera parameters
     * 
     * @param focalLength The focal length of the camera
     * @param xResolution The resolution of the camera in the x-direction
     * @param yResolution The resolution of the camera in the y-direction
     * @param xCenter,yCenter The "principal point" of the camera. 
     * @param xPixelPitch The physical size of a pixel in the x-direction (in meters)
     * @param yPixelPitch The physical size of a pixel in the y-direction (in meters)
     */
    Camera(decimal focalLength,
           int xResolution, int yResolution,
           decimal xCenter, decimal yCenter,
           decimal xPixelPitch, decimal yPixelPitch);

    /**
     * Creates a Camera object off of ideal Camera parameters
     * 
     * @param focalLength The focal length of the camera
     * @param pixelSize The physical size of a pixel (in meters)
     * @param xResolution The resolution of the camera in the x-direction
     * @param yResolution The resolution of the camera in the y-direction
     * 
     * @note In an ideal camera, the x and y centers are just half the resolution, 
     * but physical cameras often have a bit of offset.
     */
    Camera(decimal focalLength, decimal pixelSize, int xResolution, int yResolution);

    /**
    * Converts from a 3D point in space expressed in the camera coordiante system
    * to a 2D point in pixel coordinates.
    * 
    * @param vector A 3D vector to convert to a vector on the camera
    * 
    * @return The 2D Vector that represents the 3D vector on the camera
    * 
    * @note Assumes that Z is the depth direction and that it points away
    * from the center of the sensor. Coordinate system: X=right, Y=up, Z=depth.
    * Any vector (0, 0, z) will be at (xResolution/2, yResolution/2) on the sensor.
    */
    Vec2 SpatialToPixelCoordinates(const Vec3 &) const;

    /**
    * Maps a 2D point in pixel coordinates (row and column index of image matrix) to
    * a point on the Image plane located at z=1 in 3D space.
    * 
    * @param vector The vector on the camera to convert to a 3D vector
    * 
    * @return A vector in 3d space corresponding to the given vector, with z-component equal to 1
    * 
    * @note Not all vectors returned by this function will necessarily have the same magnitude.
    * 
    * @warning Other functions rely on the fact that returned vectors are placed one unit away 
    * (z-component equal to 1). Don't change this behavior!
    */
    Vec3 PixelToImageCoordinates(const Vec2 &) const;

    /**
     * Evaluates whether a vector can be seen in the camera
     * 
     * @param vector The vector to evaluate
     * 
     * @return true iff vector could be seen in this camera
     */
    bool InSensor(const Vec2 &vector) const;

    /**
     * Expose camera calibration matrix (intrinsic matrix) 
     *
     * @return The camera calibration matrix
     */
    const Mat3& GetCalibrationMatrix() const { return calibrationMatrix; }

    /**
     * Expose inverse camera calibration matrix (intrinsic matrix) 
     *
     * @return The inverse camera calibration matrix
     */ 
    const Mat3& GetInverseCalibrationMatrix() const { return inverseCalibrationMatrix; }

 private:
   /**
     * Computes and initializes the calibration matrix and the inverse calibration matrix 
     * from camera parameters.
     * 
     * @post calibrationMatrix and inverseCalibrationMatrix are initialized and ready to use.
     */
    void initializeCalibrationMatrixes();

    // TODO: distortion
    /// The focal length (m)
    decimal focalLength;
    /// The x resolution (pixels)
    int xResolution;
    /// The y resolution (pixels)
    int yResolution;
    /// The x center (pixels)
    decimal xCenter;
    /// The y center (pixels)
    decimal yCenter;
    /// The x pixel pitch (m)
    decimal xPixelPitch;
    /// The y pixel pitch (m)
    decimal yPixelPitch;
    /// The camera calibration matrix
    Mat3 calibrationMatrix;
    /// The inverse camera calibration matrix
    Mat3 inverseCalibrationMatrix;
};

}  // namespace found

#endif  // SRC_COMMON_SPATIAL_CAMERA_HPP_
