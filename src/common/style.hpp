#ifndef STYLE_H
#define STYLE_H

#include <vector>
#include <unordered_set>
#include <functional>
#include <utility>
#include <memory>

#include "common/spatial/attitude-utils.hpp"
#include "common/decimal.hpp"
#include "common/pipeline.hpp"

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
 * @brief Represents a set of classical orbital elements.
 *  
 * This struct stores orbital parameters such as eccentricity,
 * inclination, RAAN, etc., which are useful for orbit propagation
 * and analysis.
 * 
 */
struct OrbitalElements {
    double angularMomentum;      // Specific angular momentum (h)
    double eccentricity;       // Eccentricity (e)
    double inclination;        // Inclination (i) in radians
    double raan;               // Right Ascension of the Ascending Node (Ω) in radians
    double argumentOfPerigee;  // Argument of Perigee (ω) in radians
    double trueAnomaly;        // True Anomaly (ν) in radians
    double periapsis;         // Periapsis distance
    double apoapsis;          // Apoapsis distance
};

/**
 * @brief Represents the state of an orbiting body at a specific time.
 * 
 * This struct encapsulates the time, position, and velocity of an orbiting
 * object, which can be used for orbit propagation and analysis.
 */
struct State {
  double t;  // s
  Vec3   r;  // position (km if mu in km^3/s^2)
  Vec3   v;  // velocity (km/s)
};

/// The output for Kinematic Profile Completion. Currently set to two functions that
/// will tell you the position and velocity of the satellite at any given time
typedef std::pair<std::function<Vec3(int)>,std::function<Vec3(int)>> KinematicPrediction;

/// Pipeline for Calibration
typedef Pipeline<std::pair<EulerAngles, EulerAngles>, Quaternion> CalibrationPipeline;

/// Pipeline for Distance Determination
typedef Pipeline<Image, PositionVector> DistancePipeline;

/// Pipeline for Orbital Determination
typedef Pipeline<LocationRecords, LocationRecords> OrbitPipeline;

}  // namespace found

#endif  // STYLE_H
