#ifndef VELOCITY_H
#define VELOCITY_H

#include "common/style.hpp"
#include "common/spatial/attitude-utils.hpp"
#include "common/pipeline.hpp"

namespace found {

/**
 * A KinematicProfilingAlgorithm is a class that houses the Kinematic Profile Completion
 * Algorithm. This algorithm uses the spatial projection of the satellite's orbit, as obtained
 * from the OrbitDeterminationAlgorithm, and matches it to a time-based function predicting the
 * position of the satellite at any time
 * 
*/
class KinematicProfilingAlgorithm : public Stage<OrbitParams, KinematicPrediction> {
 public:
    // Destroys this
    virtual ~KinematicProfilingAlgorithm();
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
    KinematicPrediction Run(const OrbitParams &orbit /*Params to override the base class one*/) override;
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
    KinematicPrediction Run(const OrbitParams &orbit /*Params to override the base class one*/) override;
 private:
    // Fields specific to this algorithm go here, and helper methods
};

}  // namespace found


#endif
