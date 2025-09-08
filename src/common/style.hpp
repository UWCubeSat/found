#ifndef SRC_COMMON_STYLE_HPP_
#define SRC_COMMON_STYLE_HPP_

#include <vector>
#include <unordered_set>
#include <functional>
#include <utility>
#include <memory>

#include "common/spatial/attitude-utils.hpp"
#include "common/decimal.hpp"
#include "common/pipeline/pipelines.hpp"

namespace found {

/// Alias for very precise floating point numbers.
typedef __float128 PreciseDecimal;

/// The output for Edge Detection Algorithms (edge.hpp/cpp). Currently set
/// to a vector of 2D points on the image, according to image coordinate systems
typedef std::vector<Vec2> Points;

/// The output for Vector Assembly Algorithms (vectorize.hpp). Currently set
/// to a 3D Vector that represents the satellite's position relative to Earth's
/// coordinate system.
typedef Vec3 PositionVector;

/**
 * Represents an image
 * 
 * @note If image points to heap allocated memory,
 * it must be freed appropriately
 */
struct Image {
    /// The image width
    int width;
    /// The image height
    int height;
    /// The image channels
    int channels;
    /**
     * The image contents
     * 
     * @note For each pixel, the channels are collated, meaning that
     * for any pixel at index, the value of that pixel in channel n
     * (for 0 <= index < width * height and 0 <= n < channels) is
     * image[channels * index + n]
     */
    unsigned char *image;
};

/**
 * The Mask struct represents a binary mask used to convolve the image.
 *
 * @param width The width of the mask
 * @param height The height of the mask
 * @param channels The number of channels in the mask
 * @param center_width The center width of the mask
 * @param center_height The center height of the mask
 * @param data The matrix data of the mask
 *
 * @pre the center_width and center_height must be a
 * valid with and height 0 <= center_width < width
 * and 0 <= center_height < height. Therefore, width
 * and height > 0.
 *
 * @invariant The data pointer is not null and points to a valid
 * memory region of size (width * height * channels).
 *
 * @post The data must be properly deallocated
 *
 * @note If Mask points to heap allocated memory,
 * it must be freed appropriately
 */
struct Mask {
    /// The Mask width
    int width;
    /// The Mask height
    int height;
    /// The Mask channels
    int channels;
    /// The Mask center width
    int center_width;
    /// The Mask center height
    int center_height;
    /**
     * The Mask contents
     * 
     * @note For each weight, the channels are collated, meaning that
     * for any weight at index, the value of that weight in channel n
     * (for 0 <= index < width * height and 0 <= n < channels) is
     * image[channels * index + n]
     */
    float *data;
};

/**
 * Represents an ConvolvedOutput
 * 
 * @note If ConvolvedOutput points to heap allocated memory,
 * it must be freed appropriately
 */
struct ConvolvedOutput {
    /// The ConvolvedOutput width
    int width;
    /// The ConvolvedOutput height
    int height;
    /// The ConvolvedOutput channels
    int channels;
    /**
     * The ConvolvedOutput contents
     * 
     * @note For each pixel, the channels are collated, meaning that
     * for any pixel at index, the value of that pixel in channel n
     * (for 0 <= index < width * height and 0 <= n < channels) is
     * ConvolvedOutput[channels * index + n]
     */
    std::unique_ptr<float[]> image;
    /**
     * The ConvolvedOutput constructor
     *
     * @param w The width of the output
     * @param h The height of the output
     * @param c The number of channels in the output
     * @param data The raw image data
     * 
     * @note TODO
     */
    ConvolvedOutput(int w, int h, int c, std::unique_ptr<float[]> data) :
        width(w), height(h), channels(c), image(std::move(data)) {}
};

/**
 * Represents a 2D edge in an image
 * 
 * @note This must be carried with the original
 * image, as there's no such field in this struct
 */
struct Edge {
    /// The edge points
    Points points;
    /// The lowest point (left upper edge)
    Vec2 upperLeft;
    /// The highest point (right lower edge)
    Vec2 lowerRight;
};

/// A collection of Edges
typedef std::vector<Edge> Edges;

/**
 * Represents a connected component in an image
 * 
 * @note This must be carried with the original
 * image, as there's no such field in this struct
 */
struct Component {
    /// The points in this component
    std::unordered_set<uint64_t> points;
    /// The lowest point (left upper edge)
    Vec2 upperLeft;
    /// The highest point (right lower edge)
    Vec2 lowerRight;
};

/// A collection of Image Pixels
typedef std::vector<Component> Components;

/**
 * @brief Represents a single spatial data point with position and timestamp.
 */
struct LocationRecord {
    /**
     * @brief Timestamp associated with the position, in microseconds or appropriate units.
     */
    uint64_t timestamp;

    /**
     * @brief 3D position of the recorded data point.
     */
    Vec3 position;
};

// so that we don't have to copy the data.
/// A collection of Location Records
typedef std::vector<LocationRecord> LocationRecords;

/**
 * OrbitParams defines the orbital
 * parameters of a given orbit
 */
struct OrbitParams {
    /// The initial position of the satellite with respect to Earth
    /// (at t = 0 and theta = 0)
    Vec3 initialCondition;

    /**
     * The position of the satellite as a function of the number of revolutions
     * it has done
     * 
     * @param theta The number of revolutions the satellite has done
     * 
     * @return The position vector after theta revolutions have taken place
     * */
    Vec3 (* position)(int theta);

    /**
     * The major axis vector as a function with respect to the revolutions
     * of the satellite
     * 
     * @param theta The number of revolutions the satellite has done
     * 
     * @return The major axis vector after theta revolutions have taken
     * place
     * */
    Vec3 (* majorAxis)(int theta);

    /**
     * The minor axis vector as a function with respect to the revolutions
     * of the satellite
     * 
     * @param theta The number of revolutions the satellite has done
     * 
     * @return The minor axis vector after theta revolutions have taken
     * place
     * */
    Vec3 (* minorAxis)(int theta);

    /**
     * The angular speed that the satellite's orbit plane rotates in plane at with respect
     * to the number of revolutions in the satellite's orbit. This is the same as
     * the speed at which the major axis rotates around the barycenter of orbit in the plane
     * of orbit, which is approximately at Earth's center
     * 
     * @param theta The number of revolutions the satellite has done
     * 
     * @return The angular speed at which the plane of orbit rotates at, in plane, after theta 
     * revolutions
     * */
    decimal (* inPlaneRotation)(int theta);

    /**
     * The angular speed that the satellite's orbit plane rotates out of plane at with 
     * respect to the number of revolutions in the satellite's orbit. This is the same as
     * the angular speed at which the plane of orbit changes
     * 
     * @param theta The number of revolutions the satellite has done
     * 
     * @return The angular speed at which the orbital plane rotates at, out of plane, after
     * theta revolutions
     * */
    decimal (* outPlaneRotation)(int theta);
};

/// The output for Orbit Trajectory Calculation Algorithms. Currently set to
/// a struct that holds the orbit equations.
typedef struct OrbitParams OrbitParams;

/// The output for Kinematic Profile Completion. Currently set to two functions that
/// will tell you the position and velocity of the satellite at any given time
typedef std::pair<std::function<Vec3(int)>,std::function<Vec3(int)>> KinematicPrediction;

/// Number of (maximum) stages for each pipeline
constexpr size_t calibration_size = 1;
constexpr size_t distance_size = 3;
constexpr size_t orbit_size = 2;

/// Pipeline for Calibration
typedef SequentialPipeline<std::pair<EulerAngles, EulerAngles>, Quaternion, calibration_size> CalibrationPipeline;

/// Pipeline for Distance Determination
typedef SequentialPipeline<Image, PositionVector, distance_size> DistancePipeline;

/// Pipeline for Orbital Determination
typedef SequentialPipeline<LocationRecords, LocationRecords, orbit_size> OrbitPipeline;

}  // namespace found

#endif  // SRC_COMMON_STYLE_HPP_
