#ifndef SRC_DISTANCE_OUTPUT_HPP_
#define SRC_DISTANCE_OUTPUT_HPP_

#include "common/spatial/attitude-utils.hpp"
#include "common/decimal.hpp"

namespace found {

    /**
     * Converts a celestial vector to Earth-Centered, Earth-Fixed (ECEF) coordinates.
     * 
     * @param celestialVector The celestial vector to convert (in meters), must be in the
     *                        celestial (inertial) coordinate system
     * @param gmst The current Greenwich Mean Sidereal Time value, in degrees
     * 
     * @return An ECEFCoordinates representing the ECEF coordinates (x, y, z) in meters.
     *         The coordinates are in the Earth-Centered, Earth-Fixed frame where:
     *         - x-axis points to the intersection of the equator and prime meridian (0° lat, 0° lon)
     *         - y-axis points to 90°E longitude on the equator (0° lat, 90°E)
     *         - z-axis points to the North Pole (90°N lat)
     * 
     * @pre celestialVector must be in the frame of the celestial (inertial)
     *      coordinate system NOT the camera coordinate system
     */
    ECEFCoordinates GetEarthCoordinates(Vec3 &celestialVector, decimal gmst);

}  // namespace found


#endif  // SRC_DISTANCE_OUTPUT_HPP_
