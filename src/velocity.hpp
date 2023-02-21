#ifndef VELOCITY_H
#define VELOCITY_H

#include "style.hpp"
#include "attitude-utils.hpp"

namespace found {

// Always name whatever your return type is this type
typedef Vec3 VelocityPrediction;

class VelocityPredictionAlgorithm {
public:
    VelocityPredictionAlgorithm();
    virtual ~VelocityPredictionAlgorithm();
    virtual VelocityPrediction Run(/*Params common to this type*/) = 0;
private:

};


class EulerianVelocityPredictionAlgorithm : public VelocityPredictionAlgorithm {
public:
    EulerianVelocityPredictionAlgorithm(/*Params to initialze fields for this object*/);
    ~EulerianVelocityPredictionAlgorithm();
    VelocityPrediction Run(/*Params to override the base class one*/) override;
private:
    // Fields specific to this algorithm go here
};

// This one is complicated when it gets to
class KeplerVelocityPredictionAlgorithm : public VelocityPredictionAlgorithm {
public:
    KeplerVelocityPredictionAlgorithm(/*Params to initialze fields for this object*/);
    ~KeplerVelocityPredictionAlgorithm();
    VelocityPrediction Run(/*Params to override the base class one*/) override;
private:
    // Fields specific to this algorithm go here 
};

}


#endif