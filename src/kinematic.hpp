#ifndef VELOCITY_H
#define VELOCITY_H

#include "style.hpp"
#include "attitude-utils.hpp"

namespace found {

/**
 * A KinematicProfilingAlgorithm is a class that houses the Kinematic Profile Completion
 * Algorithm. This algorithm uses the spatial projection of the satellite's orbit, as obtained
 * from the OrbitDeterminationAlgorithm, and matches it to a time-based function predicting the
 * position of the satellite at any time
 * 
*/
class KinematicProfilingAlgorithm {
public:
    /**
     * Creates a KinematicProfilingAlgorithm object
     * 
    */
    KinematicProfilingAlgorithm();

    // Destroys this
    virtual ~KinematicProfilingAlgorithm();

    /**
     * Derives the kinematic profile of the satellite
     * 
     * @param orbit The projected path of the satellite
     * 
     * @return A KinematicPrediction that describes the kinematic profile of the satellite
     * 
    */
    virtual KinematicPrediction Run(OrbitParams &orbit /*Params common to this type*/) = 0;
};

/**
 * A KeplerKinematicProfilingAlgorithm is a class that houses the Kinematic Profile Completion
 * Algorithm. This algorithm uses the spatial projection of the satellite's orbit, as obtained
 * from the OrbitDeterminationAlgorithm, and matches it to a time-based function predicting the
 * position of the satellite at any time. It uses Kepler's Second Law to approximate the
 * velocity profile of the satellite
 * 
*/
class EulerianKinematicProfilingAlgorithm : public KinematicProfilingAlgorithm {
public:

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    EulerianKinematicProfilingAlgorithm(/*Params to initialze fields for this object*/);

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    ~EulerianKinematicProfilingAlgorithm();

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    KinematicPrediction Run(OrbitParams &orbit /*Params to override the base class one*/) override;
private:
    // Fields specific to this algorithm go here, and helper methods
};

/**
 * A KeplerKinematicProfilingAlgorithm is a class that houses the Kinematic Profile Completion
 * Algorithm. This algorithm uses the spatial projection of the satellite's orbit, as obtained
 * from the OrbitDeterminationAlgorithm, and matches it to a time-based function predicting the
 * position of the satellite at any time. It uses Kepler's Second Law to accurately obtain the
 * velocity profile of the satellite
 * 
*/
class KeplerKinematicProfilingAlgorithm : public KinematicProfilingAlgorithm {
public:

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    KeplerKinematicProfilingAlgorithm(/*Params to initialze fields for this object*/);

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    ~KeplerKinematicProfilingAlgorithm();

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    KinematicPrediction Run(OrbitParams &orbit /*Params to override the base class one*/) override;
private:
    // Fields specific to this algorithm go here, and helper methods
};

}


#endif