#ifndef EDGE_H
#define EDGE_H

#include <vector>
#include "style.hpp"

namespace found {

struct Point {
    decimal x;
    decimal y;
};

// Always name your output Points
typedef std::vector<Point> Points;

class EdgeDetectionAlgorithm {
public:
    EdgeDetectionAlgorithm();
    
    // This is your core algorithm. I made a mock return type, feel free to redesign it!
    virtual Points Run(char* image/*parameters all algorithms will need (Override this plz)*/);
private:
    //useful fields for all algorithms

};

class LoCEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
public:
    LoCEdgeDetectionAlgorithm(/*Put more fields here!*/);
// Overrided Go() goes here
    Points Run(char* imgae/*parameters all algorithms will need (Override this plz)*/) override;
private:
// useful fields specific to this algorhtm
};

class SimpleEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
public:
// constructor goes here
// Overrided Go() goes here
private:
// useful fields specific to this algorhtm
};

}

#endif