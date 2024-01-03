#ifndef CONNECTCOMP_H
#define CONNECTCOMP_H

#include <vector>
#include <unordered_map>
#include <string>

namespace found {

struct Point {
    int x;
    int y;
};

struct Edge {
    int magnitude; // number of points in this edge
    std::vector<Point> points;
};

// Function prototype
std::vector<Edge> NaiveConnectedComponent(unsigned char *image, int imageWidth, int imageHeight);

}

#endif
