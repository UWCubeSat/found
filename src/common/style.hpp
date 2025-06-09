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
 * Represents a connected component in an image
 * 
 * @note This must be carried with the original
 * image, as there's no such field in this struct
 */
struct Component {
    /// The points in this component
    std::vector<uint64_t> points;
    /// The lowest point (left upper edge)
    Vec2 upperLeft;
    /// The highest point (right lower edge)
    Vec2 lowerRight;
};

/// A collection of Image Pixels
typedef std::vector<Component> Components;

// TODO: Merge with Data Serialization branch
/**
 * @brief Represents a single location record containing a position and a timestamp.
 */
struct LocationRecord {
    /**
     * @brief The timestamp corresponding to this location (in microseconds or appropriate unit).
     */
    uint64_t timestamp;
    /**
     * @brief The 3D position vector associated with this location.
     */
    Vec3 position;
};

// TODO: Maybe change this to line up with the DataFile struct
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

/// Pipeline for Calibration
typedef Pipeline<std::pair<EulerAngles, EulerAngles>, Quaternion> CalibrationPipeline;

/// Pipeline for Distance Determination
typedef Pipeline<Image, PositionVector> DistancePipeline;

/// Pipeline for Orbital Determination
typedef Pipeline<LocationRecords, LocationRecords> OrbitPipeline;

}  // namespace found

#endif  // STYLE_H
