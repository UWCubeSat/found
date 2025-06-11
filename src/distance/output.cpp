#include "distance/output.hpp"

#include <ctime>

#include "common/spatial/attitude-utils.hpp"
#include "common/time/time.hpp"
#include "common/decimal.hpp"

namespace found {

EarthSphericalVec3 GetEarthCoordinates(Vec3 &celestialVector, decimal gmst) {
    // Converts epoch time to GMST in degrees, then we convert to radians
    // We should ensure Euclidean Mod, but both the divisor and dividend
    // are positive, so we don't need it (GMST > 0 after Jan 1st, 2000).
    decimal GMST = std::fmod(DECIMAL_M_PI * gmst / 180.0, 2 * DECIMAL_M_PI);
    // Figure out Earth's Rotating Frame and express the position in that frame
    Quaternion toEarthRotatingFrame = SphericalToQuaternion(GMST, 0, 0);
    Vec3 position = toEarthRotatingFrame.Rotate(celestialVector);

    // Figure out the right ascension and declination of the vector
    decimal RA = std::atan2(position.y, position.x);  // Huh, the range is [-PI, PI], not [0, 2PI]. That's convenient
    decimal DE = std::asin(position.Normalize().z);  // Range is [-PI/2, PI/2]

    // Longitude, Lattitude and Altitude Follow, with conversion
    // to degrees and range adjustment from RA to longitude
    return {180.0 * RA / DECIMAL_M_PI,
            180 * DE / DECIMAL_M_PI,
            position.Magnitude(),
            gmst};
}

}  // namespace found
