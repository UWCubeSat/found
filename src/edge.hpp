#ifndef EDGE_H
#define EDGE_H

#include <vector>

struct Point {
    int x;
    int y;
};

typedef std::vector<Point> Points;

class EdgeDetectionAlgorithm {
public:
    EdgeDetectionAlgorithm(char* picture /*Put more fields here!*/) : image(picture) /*Put more initializations here*/{};
    
    // This is your core algorithm. I made a mock return type, feel free to redesign it!
    virtual Points Go(/*parameters all algorithms will need (Override this plz)*/);
private:
    char* image;
    //useful fields for all algorithms

};

class LoCEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
public:
    LoCEdgeDetectionAlgorithm(char* picture /*Put more fields here!*/) : EdgeDetectionAlgorithm(picture) /*Put more initializations here*/{};
// Overrided Go() goes here
    Points Go(/*parameters all algorithms will need (Override this plz)*/) override;
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


#endif