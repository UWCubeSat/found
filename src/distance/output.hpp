#ifndef SRC_DISTANCE_OUTPUT_HPP_
#define SRC_DISTANCE_OUTPUT_HPP_

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
        /// Radius NOT Altitude (r, meters)
        decimal radius;
        /// Greenwich Mean Sidereal Time (degrees)
        decimal GMST;
    };

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
    EarthSphericalVec3 GetEarthCoordinates(Vec3 &equatorialVector, decimal gmst);

    // NOTE: I purposefully don't include a GetEarthCoordinates(Vec3 &equatorialVector).
    // This is becuase equatorialVector and gmst are derived at different times, though
    // they originate from the same time point.

}  // namespace found


#endif  // SRC_DISTANCE_OUTPUT_HPP_
