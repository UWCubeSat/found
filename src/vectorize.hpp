#ifndef VECTORIZE_H
#define VECTORIZE_H

#include "attitude-utils.hpp"
#include "style.hpp"

namespace found {

/**
 * The VectorGenerationAlgorithm class houses the Vector Assembly Algorithm. This algorithm
 * finds the position from Earth with respect to its center with a 3D Vector (Vec3).
 * 
*/
class VectorGenerationAlgorithm {
public:

    /**
     * Creates a VectorGenerationAlgorithm object
    */
    VectorGenerationAlgorithm();

    // Destroys this
    virtual ~VectorGenerationAlgorithm();

    /**
     * Runs the Vector Assembly Algorithm, which finds the vector of the satellite with respect
     * to Earth's center
     * 
     * @param x_E The distance from Earth
     * 
     * @return A PositionVector that represents the 3D Vector of the satellite relative to
     * Earth's center
    */
    virtual PositionVector Run(distFromEarth x_E /*Params common to this type*/) = 0;
};

/**
 * The LOSTVectorGenerationAlgorithm class houses the a Vector Assembly Algorithm that calculates the
 * position of the satellite using orientation information determined from LOST.
 * 
*/
class LOSTVectorGenerationAlgorithm : public VectorGenerationAlgorithm {
public:

    /**
     * Creates a LOSTVectorGenerationAlgorithm object
     * 
     * @param orientation The orientation of the satellite as determined by LOST
    */
    LOSTVectorGenerationAlgorithm(Vec3 orientation/*Params to initialze fields for this object*/) : orientation(orientation) {};
    
    // Destroys this
    ~LOSTVectorGenerationAlgorithm();

    /**
     * Runs the Vector Assembly Algorithm, which finds the vector of the satellite with respect
     * to Earth's center using information from LOST
     * 
     * @param x_E The distance from Earth
     * 
     * @return A PositionVector that represents the 3D Vector of the satellite relative to
     * Earth's center
    */
    PositionVector Run(distFromEarth x_E /*Params to override the base class one*/) override;
private:
    // Fields specific to this algorithm go here

    // Orientation from LOST
    Vec3 orientation;
};


class FeatureDetectionVectorGenerationAlgorithm : public VectorGenerationAlgorithm {
public:
    FeatureDetectionVectorGenerationAlgorithm(/*Params to initialze fields for this object*/);
    ~FeatureDetectionVectorGenerationAlgorithm();
    PositionVector Run(distFromEarth x_E /*Params to override the base class one*/) override;
private:
    // Fields specific to this algorithm go here 
};

}

#endif