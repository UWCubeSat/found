/**
 * This file controls all stylistic-related choices for FOUND (except for decimal)
 * 
*/

#ifndef STYLE_H
#define STYLE_H

#include <vector>
#include <functional>
#include <utility>

#include "spatial/attitude-utils.hpp"
#include "style/decimal.hpp"

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

}  // namespace found

#endif
