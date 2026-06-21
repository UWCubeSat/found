#include "distance/output.hpp"

#include <ctime>

#include "common/spatial/attitude-utils.hpp"
#include "common/time/time.hpp"
#include "common/decimal.hpp"

namespace found {

ECEFCoordinates GetEarthCoordinates(Vec3 &equatorialVector, decimal gmst) {
    // Converts GMST from degrees to radians
    // We should ensure Euclidean Mod, but both the divisor and dividend
    // are positive, so we don't need it (GMST > 0 after Jan 1st, 2000).
    // The fmod ensures the result is in [0, 2π) radians
    decimal GMST = std::fmod(DECIMAL_M_PI * gmst / DECIMAL(180.0), 2 * DECIMAL_M_PI);

    // Convert from equatorial (inertial) frame to Earth-Centered, Earth-Fixed (ECEF) frame
    // Standard ECEF convention requires rotation by -GMST around Z-axis.
    Quaternion toEarthRotatingFrame = SphericalToQuaternion(GMST, 0, 0).conjugate();
    Vec3 position = toEarthRotatingFrame * equatorialVector;

    return ECEFCoordinates(position);
}

EarthSphericalVec3 GetEarthLLACoordinates(Vec3 &equatorialVector, decimal gmst) {
    Vec3 position = GetEarthCoordinates(equatorialVector, gmst);

    // Figure out the right ascension and declination of the vector
    // Range is [-PI, PI], not [0, 2PI]. That's convenient
    decimal RA = std::atan2(position.y(), position.x());
    decimal DE = std::asin(position.normalized().z());  // Range is [-PI/2, PI/2]

    // Longitude, Lattitude and Altitude Follow, with conversion
    // to degrees and range adjustment from RA to longitude
    return {RadToDeg(RA),
            RadToDeg(DE),
            position.norm(),
            gmst};
}

}  // namespace found
