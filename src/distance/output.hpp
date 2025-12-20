#ifndef SRC_DISTANCE_OUTPUT_HPP_
#define SRC_DISTANCE_OUTPUT_HPP_

#include "common/spatial/attitude-utils.hpp"
#include "common/decimal.hpp"

namespace found {

    /**
     * Converts a celestial vector to Earth-Centered, Earth-Fixed (ECEF) coordinates.
     * 
     * This function takes a position vector in the celestial coordinate system and
     * converts it to ECEF coordinates using the WGS84 ellipsoid. The conversion involves:
     * 1. Rotating the vector to Earth's rotating frame using GMST
     * 2. Extracting geodetic coordinates (latitude, longitude, altitude)
     * 3. Converting to ECEF coordinates using the WGS84 ellipsoid
     * 
     * @param celestialVector The celestial vector to convert (in meters)
     * @param gmst The current Greenwich Mean Sidereal Time value, in degrees
     * 
     * @return An ECEFCoordinates representing the ECEF coordinates (x, y, z) in meters.
     *         The coordinates are in the Earth-Centered, Earth-Fixed frame where:
     *         - x-axis points to the intersection of the equator and prime meridian
     *         - y-axis points to 90°E longitude on the equator
     *         - z-axis points to the North Pole
     * 
     * @pre celestialVector must be in the frame of the celestial
     *      coordinate system NOT the camera coordinate system
     */
    ECEFCoordinates GetEarthCoordinates(Vec3 &celestialVector, decimal gmst);

}  // namespace found


#endif  // SRC_DISTANCE_OUTPUT_HPP_
