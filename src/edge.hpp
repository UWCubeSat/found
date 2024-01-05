#ifndef EDGE_H
#define EDGE_H

#include "style.hpp"

namespace found {

/**
 * The EdgeDetection Algorithm class houses the Edge Detection Algorithm. This algorithm uses 
 * a picture of Earth and finds all points on the horizon within the picture.
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
     * Finds all points on Earth's horizon as seen in an image
     * 
     * @param image The image of Earth, represented as a character array with values from 0-255
     * that represents the black/white color of each pixel
     * 
     * @return A Points object that holds all points found in the image
    */
    virtual Points Run(unsigned char* image/*parameters all algorithms will need (Override this plz)*/) = 0;
};

/**
 * The SimpleEdgeDetection Algorithm class houses the Edge Detection Algorithm. This algorithm uses 
 * a picture of Earth and finds all points on the horizon within the picture by employing thresholding
 * to filter out edge components
*/
class SimpleEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
public:
    
    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    SimpleEdgeDetectionAlgorithm(/*Put more fields here!*/);

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    virtual ~SimpleEdgeDetectionAlgorithm(/*Put more fields here!*/);

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    Points Run(unsigned char* image/*parameters all algorithms will need (Override this plz)*/) override;
private:
    // useful fields specific to this algorithm and helper methods
};

/**
 * The LoGEdgeDetection Algorithm class houses the Edge Detection Algorithm. This algorithm uses 
 * a picture of Earth and finds all points on the horizon within the picture by employing a 
 * Laplacian of Gaussian (LoC) filter to the image.
*/
class LoCEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
public:

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    LoCEdgeDetectionAlgorithm(/*Put more fields here!*/);

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    virtual ~LoCEdgeDetectionAlgorithm(/*Put more fields here!*/);

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    Points Run(unsigned char* image/*parameters all algorithms will need (Override this plz)*/) override;
private:
    // useful fields specific to this algorithm and helper methods
};

}

#endif