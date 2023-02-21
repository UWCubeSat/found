#ifndef VECTORIZE_H
#define VECTORIZE_H

#include "attitude-utils.hpp"
#include "style.hpp"

namespace found {

typedef Vec3 PositionVector;

class VectorGenerationAlgorithm {
public:
    VectorGenerationAlgorithm();
    virtual ~VectorGenerationAlgorithm();
    virtual PositionVector Run(/*Params common to this type*/) = 0;
private:

};


class LOSTVectorGenerationAlgorithm : public VectorGenerationAlgorithm {
public:
    LOSTVectorGenerationAlgorithm(/*Params to initialze fields for this object*/);
    ~LOSTVectorGenerationAlgorithm();
    PositionVector Run(/*Params to override the base class one*/) override;
private:
    // Fields specific to this algorithm go here
};


class FeatureDetectionVectorGenerationAlgorithm : public VectorGenerationAlgorithm {
public:
    FeatureDetectionVectorGenerationAlgorithm(/*Params to initialze fields for this object*/);
    ~FeatureDetectionVectorGenerationAlgorithm();
    PositionVector Run(/*Params to override the base class one*/) override;
private:
    // Fields specific to this algorithm go here 
};

}

#endif