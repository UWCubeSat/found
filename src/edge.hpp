#ifndef EDGE_H
#define EDGE_H

#include <vector>
#include "style.hpp"

namespace found {

/**
 * The EdgeDetection Algorithm class houses the Edge Detection Algorithm. This algorithm uses 
 * a picture of  Earth and finds all points on the horizon within the picture.
*/
class EdgeDetectionAlgorithm {
public:
    /**
     * Creates an EdgeDetectionAlgorithm object
     * 
     * @note Constructs an Edge Detection Algorithm
    */
    EdgeDetectionAlgorithm();

    // Destroys this
    virtual ~EdgeDetectionAlgorithm();
    
    /**
     * Runs the Edge Detection Algorithm, which finds all points on Earth's horizon.
     * 
     * @param image The image of Earth, represented as a character array with values from 0-255
     * that represents the black/white color of each pixel
     * 
     * @return A Points object that holds all points found in the image
    */
    virtual Points Run(char* image/*parameters all algorithms will need (Override this plz)*/) = 0;
};

class LoCEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
public:
    LoCEdgeDetectionAlgorithm(/*Put more fields here!*/);
    virtual ~LoCEdgeDetectionAlgorithm(/*Put more fields here!*/);
// Overrided Go() goes here
    Points Run(char* imgae/*parameters all algorithms will need (Override this plz)*/) override;
private:
// useful fields specific to this algorithm and helper methods
};

class SimpleEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
public:
// constructor goes here
// Overrided Go() goes here
private:
// useful fields specific to this algorithm and helper methods
};

}

#endif