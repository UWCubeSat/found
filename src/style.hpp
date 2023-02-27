#include <vector>
#include <functional>

#include "attitude-utils.hpp"

namespace found {

// For controlling floating-point type memory usage throughout the program
typedef float decimal;

// The output for Edge Detection Algorithms (edge.hpp/cpp). Currently set 
// to a vector of 2D points on the image, according to image coordinate systems
typedef std::vector<Vec2> Points;

// The output for Distance Determination Algorithms (distance.hpp/cpp). Currently 
// set to a floating point value that represents the distance from Earth
typedef decimal distFromEarth;

// The output for Vector Assembly Algorithms (vectorize.hpp). Currently set
// to a 3D Vector that represents the satellite's position relative to Earth's
// coordinate system.
typedef Vec3 PositionVector;

struct OrbitParams {

    // The initial position of the satellite with respect to Earth 
    // (at t = 0 and theta = 0)
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

// The output for Orbit Trajectory Calculation Algorithms. Currently set to
// a struct that holds the orbit equations.
typedef struct OrbitParams OrbitParams;

// The output for Kinematic Profile Completion. Currently set to a singular
// function that will tell you the position of the satellite at any given time
typedef std::function<Vec3(int)> VelocityPrediction;
}