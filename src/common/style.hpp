/**
 * This file controls all stylistic-related choices for FOUND (except for decimal)
 * 
*/

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

/// The output for Edge Detection Algorithms (edge.hpp/cpp). Currently set
/// to a vector of 2D points on the image, according to image coordinate systems
typedef std::vector<Vec2> Points;

/// The output for Distance Determination Algorithms (distance.hpp/cpp). Currently
/// set to a floating point value that represents the distance from Earth
typedef decimal distFromEarth;

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
    /// The image contents
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
 * OrbitParams defines the orbital
 * parameters of a given orbit
 */
struct OrbitParams {
    double h;    /**< Specific angular momentum magnitude */
    double e;    /**< Eccentricity magnitude */
    double RA;   /**< Right Ascension of the Ascending Node (RAAN) */
    double incl; /**< Inclination */
    double w;    /**< Argument of Periapsis */
    double TA;   /**< True Anomaly */
};

/// The output for Orbit Trajectory Calculation Algorithms. Currently set to
/// a struct that holds the orbit equations.
typedef struct OrbitParams OrbitParams;

/// The output for Kinematic Profile Completion. Currently set to two functions that
/// will tell you the position and velocity of the satellite at any given time
typedef std::pair<std::function<Vec3(int)>,std::function<Vec3(int)>> KinematicPrediction;

/// Pipeline for Calibration
typedef Pipeline<std::pair<EulerAngles, EulerAngles>, Quaternion> CalibrationPipeline;

/// Pipeline for Distance Determination
typedef Pipeline<Image, PositionVector> DistancePipeline;

/// Pipeline for Orbital Determination [TODO(nguy8tri): Replace this statement after merge with Data Serialization]
typedef Pipeline<std::vector<PositionVector>, OrbitParams> OrbitPipeline;

}  // namespace found

#endif  // STYLE_H
