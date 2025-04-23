#ifndef EDGE_H
#define EDGE_H

#include "common/style.hpp"
#include "common/pipeline.hpp"

namespace found {

/**
 * The EdgeDetection Algorithm class houses the Edge Detection Algorithm. This algorithm uses 
 * a picture of Earth and finds all points on the horizon within the picture.
*/
class EdgeDetectionAlgorithm : public Stage<Image, Points> {};

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
    Points Run(const Image &image/*parameters all algorithms will need (Override this plz)*/) override;
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
    Points Run(const Image &image/*parameters all algorithms will need (Override this plz)*/) override;
 private:
    // useful fields specific to this algorithm and helper methods
};

}  // namespace found

#endif
