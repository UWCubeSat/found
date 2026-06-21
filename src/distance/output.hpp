#ifndef SRC_DISTANCE_OUTPUT_HPP_
#define SRC_DISTANCE_OUTPUT_HPP_

#include "common/spatial/attitude-utils.hpp"
#include "common/decimal.hpp"

namespace found {

/**
 * Represents Earth coordinates in spherical form (Longitude, Latitude, Altitude)
 * with associated GMST value
 */
struct EarthSphericalVec3 {
    /// Longitude in degrees
    decimal longitude;
    /// Latitude in degrees
    decimal latitude;
    /// Altitude in meters (distance from Earth's center)
    decimal altitude;
    /// Greenwich Mean Sidereal Time in degrees
    decimal gmst;
};

    /**
     * Converts a equatorial vector to Earth-Centered, Earth-Fixed (ECEF) coordinates.
     * 
     * @param equatorialVector The equatorial vector to convert (in meters)
     * @param gmst The current Greenwich Mean Sidereal Time value, in degrees
     * 
     * @return An ECEFCoordinates representing the ECEF coordinates (x, y, z) in meters.
     *         The coordinates are in the Earth-Centered, Earth-Fixed frame where:
     *         - x-axis points to the intersection of the equator and prime meridian (0° lat, 0° lon)
     *         - y-axis points to 90°E longitude on the equator (0° lat, 90°E)
     *         - z-axis points to the North Pole (90°N lat)
     * 
     * @pre equatorialVector must be in the frame of the equatorial (inertial)
     *      coordinate system NOT the camera coordinate system
     */
    ECEFCoordinates GetEarthCoordinates(Vec3 &equatorialVector, decimal gmst);

    /**
     * Obtains a equatorial vector within Earth's Rotating Frame,
     * in longitude/lattitude/altitude
     * 
     * @param equatorialVector The equatorial vector to convert
     * @param gmst The current GMST value, in degrees
     * 
     * @return An EarthSphericalVec3 describing equatorialVector
     * within Earth's Rotating Frame at time GMST
     * 
     * @pre equatorialVector must be in the frame of the equatorial
     * coordinate system NOT the camera coordinate system
     */
    EarthSphericalVec3 GetEarthLLACoordinates(Vec3 &equatorialVector, decimal gmst);

}  // namespace found


#endif  // SRC_DISTANCE_OUTPUT_HPP_
