#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vector>
#include <iostream>

#include <unordered_map>

#include <unordered_set>
#include <string.h>
#include <cassert>
#include <cstdlib>

namespace found {

struct Point {
	int x;
	int y;
};

struct Edge {
	int magnitude; // number of points in this edge
	std::vector<Point> points;
	// int Xmin, Xmax, Ymin, Ymax;
};

std::vector<Edge> NaiveConnectedComponent(unsigned char *image, int imageWidth, int imageHeight) { // const??
	int threshold = 3; // over threshold means possible components exist

	// Tells us if certian numbers in the below array are part of the same component
    std::unordered_map<int, int> equivalencies;

	// make an array with the same dimensions of the image, but it shows connected components using numbers
    unsigned char *edges = (unsigned char *) std::malloc(imageWidth * imageHeight * sizeof(unsigned char));
    edges[0] = (image[0] > threshold) ? 1 : 0;
    std::cout << static_cast<int>(edges[0]) << std::endl;
    int L = edges[0];

    // for (int i = 0; i < imageHeight; ++i) {
    //     for (int j = 0; j < imageWidth; ++j) {
    //         std::cout << "(" << static_cast<int>(edges[i * imageWidth + j]) << ") ";
    //     }
    //     std::cout << std::endl;
    // }
    

	// Initial labeling of centroids
    for(long i = 1; i < imageHeight * imageWidth; i++) {
        // This is specific to the centroiding algo, which tests if this is even part of a centroid
        if(image[i] > threshold) {

            // The indicies of the entries to the left and above i
            int up = i - imageWidth;
            int left = i - 1;
			int upLeft = i - imageWidth - 1;
			int upRight = i - imageWidth + 1;

            // Tests to see if the upper or left entry is a valid component (part of a star aka it was initialized
            // with a L value at some point)

			// Is there concern about negative indexing?
            bool leftEq = edges[left] != 0;
            bool upEq = edges[up] != 0;

            std::cout << "(" << leftEq << "," << upEq << ")";
			bool upLeftEq = edges[upLeft] != 0;
            bool upRightEq = edges[upRight] != 0;

			// Are we assuming image is of two colors?

            if(i / imageWidth == 0) { // If i is part of a top pixel, test only the left pixel
                if(leftEq) { // If the left pixel is part of a centroid, it must be part of the same star as i, if not, mark with new L value
                    edges[i] = edges[left];
                } else {
                    edges[i] = ++L;
                }
            } else if(i % imageWidth == 0) { // If i is part of a left pixel, test the upper pixel and top right pixel
                if(upEq && upRightEq && edges[up] == edges[upRight]) { // If the upper and top right pixel is part of a centroid, it must be part of the same star as i, if not, mark with new L value
                    edges[i] = edges[up];
                } else if(upEq && upRightEq && edges[up] != edges[upRight]) {
                    edges[i] = std::min(edges[up], edges[upRight]);
                    equivalencies.insert(std::pair<int, int>(int(std::max(edges[up], edges[upRight])), int(edges[i])));
                    assert(equivalencies.find(edges[i]) == equivalencies.end());
                } else if(upEq) { // top pixel is part of
                    edges[i] = edges[up];
                } else if(upRightEq) { // top right pixel is part of
                    edges[i] = edges[upRight];
                }
                else {
                    edges[i] = ++L;
                }
            } else if (i + 1 % imageWidth == 0) { // If i is part of a right pixel, test only left and upper
                if(upLeftEq) {
					edges[i] = edges[upLeft];
				} else if(leftEq && upEq && edges[left] == edges[up]) { // If the top and left pixels are part of centroid with the same L value, i is connected to them with the same L value
                    edges[i] = edges[left];
                } else if(leftEq && upEq && edges[left] != edges[up]) { // The above, but if the left and top pixels have different L values, those different values are also part of the same centroid
                    edges[i] = std::min(edges[left], edges[up]);
                    equivalencies.insert(std::pair<int, int>(int(std::max(edges[left], edges[up])), int(edges[i])));
                    assert(equivalencies.find(edges[i]) == equivalencies.end());
                } else if(leftEq && upRightEq && edges[left] == edges[upRight]) { // left and top right
                    edges[i] = edges[left];
                } else if(leftEq && upRightEq && edges[left] != edges[upRight]) { // left and top right
                    edges[i] = std::min(edges[left], edges[upRight]);
                    equivalencies.insert(std::pair<int, int>(int(std::max(edges[left], edges[upRight])), int(edges[i])));
                    assert(equivalencies.find(edges[i]) == equivalencies.end());
                } else if(upLeftEq && upRightEq && edges[upLeft] == edges[upRight]) { // top left and top right
                    edges[i] = edges[upLeft];
                } else if(upLeftEq && upRightEq && edges[upLeft] != edges[upRight]) { // top left and top right
                    edges[i] = std::min(edges[upLeft], edges[upRight]);
                    equivalencies.insert(std::pair<int, int>(int(std::max(edges[upLeft], edges[upRight])), int(edges[i])));
                    assert(equivalencies.find(edges[i]) == equivalencies.end());
                } else if(leftEq) { // From here, we get a copy of the first two if/else if statements
                    edges[i] = edges[left];
                } else if(upEq) {
                    edges[i] = edges[up];
                } else {
                    edges[i] = ++L;
                }
            } else { // Tests for general case
				if(upLeftEq) {
					edges[i] = edges[upLeft];
				} else if(upRightEq) {
                    edges[i] = edges[upRight];
                } else if(leftEq && upEq && edges[left] == edges[up]) { // If the top and left pixels are part of centroid with the same L value, i is connected to them with the same L value
                    edges[i] = edges[left];
                } else if(leftEq && upEq && edges[left] != edges[up]) { // The above, but if the left and top pixels have different L values, those different values are also part of the same centroid
                    edges[i] = std::min(edges[left], edges[up]);
                    equivalencies.insert(std::pair<int, int>(int(std::max(edges[left], edges[up])), int(edges[i])));
                    assert(equivalencies.find(edges[i]) == equivalencies.end());
                } else if(leftEq) { // From here, we get a copy of the first two if/else if statements
                    edges[i] = edges[left];
                } else if(upEq) {
                    edges[i] = edges[up];
                } else {
                    edges[i] = ++L;
                }
            }
        }
        else {
            edges[i] = 0;
        }
    }
    std::cout << std::endl;

    for (int i = 0; i < imageHeight; ++i) {
        for (int j = 0; j < imageWidth; ++j) {
            std::cout << "(" << static_cast<int>(edges[i * imageWidth + j]) << ") ";
        }
        std::cout << std::endl;
    }

	// Get statistics of each star. This is when we process the above info
    // into groups based on the pixels having the same number (or equivalent numebers via the map)
    // params is a mapping of these centroid numbers (L values) to each "group" of pixels with statistics
    // shown in Centroid params.
    std::unordered_map<int, Edge> params; // Star # to param
    for(int i = 0; i < imageWidth * imageHeight; i++) {
        if(edges[i] != 0) {
            int edgeNumber = equivalencies.find(edges[i]) == equivalencies.end() ?
                    edges[i] : equivalencies.find(edges[i]) -> second;
            if(params.find(edgeNumber) == params.end()) { // If the entry with the L value given at stars[i] is not yet in params, then initialze one
                Edge e;
				std::vector<Point> point;
                e.magnitude = 0;
                e.points = point;
                //p.xMax = 0;
                //p.xMin = imageWidth;
                //p.yMax = 0;
                //p.yMin = imageHeight;
                //e.isValid = true;
                params.insert(std::pair<int, Edge>(edgeNumber, e));
            }
            // Obtain and update the statistics for the centroid corresponding to the L value at stars[i] or the equivalent L value
            Edge *edge_ptr = &params.at(edgeNumber);
            int row = i / imageWidth;
            int col = i % imageWidth;
            // centroid_ptr -> xCoordMagSum += col * image[i];
            // centroid_ptr -> yCoordMagSum += row * image[i];
            edge_ptr -> magnitude++;
			Point p;
			p.x = row;
			p.y = col;
			edge_ptr -> points.push_back(p);
            // centroid_ptr -> magSum += image[i];
            // if(col > centroid_ptr -> xMax) {
            //     centroid_ptr -> xMax = col;
            // }
            // if(col < centroid_ptr -> xMin) {
            //     centroid_ptr -> xMin = col;
            // }
            // if(row > centroid_ptr -> yMax) {
            //     centroid_ptr -> yMax = row;
            // }
            // if(row < centroid_ptr -> yMin) {
            //     centroid_ptr -> yMin = row;
            // }
            // if (i % imageWidth == 0 || i % imageWidth == imageWidth - 1 || i / imageWidth == 0 ||
            //     i / imageWidth == imageHeight - 1) {
            //     centroid_ptr -> isValid = false;
            // }

        }
    }
	

    std::vector<Edge> connectedPoints;

    for (const auto& pair : params) {
        connectedPoints.push_back(pair.second);
    }

    return connectedPoints;
}

}

// // Function to generate a random unsigned char value between min and max (inclusive)
// unsigned char getRandomChar(unsigned char min, unsigned char max) {
//     return static_cast<unsigned char>(rand() % (max - min + 1) + min);
// }

// // Main function to test the above function
// int main() {
//     // Seed the random number generator
//     srand(static_cast<unsigned int>(time(0)));

//     // Example 2D array (3x3)
//     int height = 5;
//     int width = 5;
//     // Allocate memory for the 2D array
//     unsigned char* image = new unsigned char[height * width];

//     // Fill the array with random values and print it
//     for (int i = 0; i < height; ++i) {
//         for (int j = 0; j < width; ++j) {
//             image[i * width + j] = getRandomChar(0, 4);
//             std::cout << "(" << static_cast<int>(image[i * width + j]) << ") ";
//         }
//         std::cout << std::endl;
//     }

//     // Call the function and get the result
//     std::vector<Edge> flattened = NaiveConnectedComponent(image, height, width);

//     std::size_t length = flattened.size();

//     std::cout << "The length of the vector is: " << length << std::endl;

//     // Print the resulting vector
//     std::cout << "Flattened array: ";
//     for (Edge value : flattened) {
//         for (Point p : value.points) {
//             std::cout << "(" << p.x << "," << p.y << ") ";
//         }
//         std::cout << std::endl;
//     }
//     std::cout << std::endl;

//     // Clean up the allocated memory
//     delete[] image;

//     return 0;
// }