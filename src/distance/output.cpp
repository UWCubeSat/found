#include "distance/output.hpp"

#include <ctime>

#include "common/spatial/attitude-utils.hpp"
#include "common/time/time.hpp"
#include "common/decimal.hpp"

namespace found {

ECEFCoordinates GetEarthCoordinates(Vec3 &celestialVector, decimal gmst) {
    // Converts GMST from degrees to radians
    // We should ensure Euclidean Mod, but both the divisor and dividend
    // are positive, so we don't need it (GMST > 0 after Jan 1st, 2000).
    // The fmod ensures the result is in [0, 2π) radians
    decimal GMST = std::fmod(DECIMAL_M_PI * gmst / DECIMAL(180.0), 2 * DECIMAL_M_PI);

    // Convert from celestial (inertial) frame to Earth-Centered, Earth-Fixed (ECEF) frame
    // Standard ECEF convention requires rotation by -GMST around Z-axis.
    Quaternion toEarthRotatingFrame = SphericalToQuaternion(GMST, 0, 0);
    Vec3 position = toEarthRotatingFrame.Rotate(celestialVector);

    return ECEFCoordinates(position);
}

}  // namespace found
