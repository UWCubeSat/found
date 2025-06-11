#ifndef DISTANCE_OUTPUT_H
#define DISTANCE_OUTPUT_H

#include "common/spatial/attitude-utils.hpp"
#include "common/decimal.hpp"

namespace found {

    /**
     * Represents an Earth-centric
     * Spherical Vector with time
     * information
     */
    struct EarthSphericalVec3 {
        /// Longitude (RA, degrees)
        decimal longitude;
        /// Lattitude (DE, degrees)
        decimal lattitude;
        /// Altitude (r, meters)
        decimal altitude;
        /// Greenwich Mean Sidereal Time (degrees)
        decimal GMST;
    };

    /**
     * Obtains a celestial vector within Earth's Rotating Frame,
     * in longitude/lattitude/altitude
     * 
     * @param celestialVector The celestial vector to convert
     * @param gmst The current GMST value, in degrees
     * 
     * @return An EarthSphericalVec3 describing celestialVector
     * within Earth's Rotating Frame at time GMST
     * 
     * @pre celestialVector must be in the frame of the celestial
     * coordinate system NOT the camera coordinate system
     */
    EarthSphericalVec3 GetEarthCoordinates(Vec3 &celestialVector, decimal gmst);

    // NOTE: I purposefully don't include a GetEarthCoordinates(Vec3 &celestialVector).
    // This is becuase celestialVector and gmst are derived at different times, though
    // they originate from the same time point.

}  // namespace found


#endif  // DISTANCE_OUTPUT_H
