#include "distance/edge.hpp"

#include <unordered_map>
#include <algorithm>
#include <memory>
#include <vector>

#include "common/style.hpp"
#include "common/decimal.hpp"

namespace found {

/**
 * Checks if a label is present in the list of adjacent labels
 * 
 * @param label The label to check
 * @param adjacentLabels The list of adjacent labels
 * @param size The size of the list
 * 
 * @return true iff label is in adjacentLabels
 */
inline bool LabelPresent(int label, int *adjacentLabels, int size) {
    if (size == 0) return false;
    for (int i = 0; i < size; i++) {
        if (adjacentLabels[i] == label) {
            return true;
        }
    }
    return false;
}

/**
 * Updates the edge with the given pixel
 * 
 * @param edge The edge to update
 * @param pixel The pixel to add
 * 
 * @pre UpdateEdge must be called in order of increasing index
 */
inline void UpdateEdge(Edge &edge, Vec2 &pixel) {
    edge.points.push_back(pixel);
    if (edge.upperLeft.x > pixel.x) edge.upperLeft.x = pixel.x;
    else if (edge.lowerRight.x < pixel.x) edge.lowerRight.x = pixel.x;
    // We skip this statement, since its impossible: if (edge.upperLeft.y > pixel.y) edge.upperLeft.y = pixel.y;
    if (edge.lowerRight.y < pixel.y) edge.lowerRight.y = pixel.y;
}

/**
 * Adds a pixel to some edge, creating a new edge if necessary
 * 
 * @param image The image to which the pixel belongs
 * @param index The index of the pixel
 * @param L The current label
 * @param adjacentLabels The labels of the adjacent edges
 * @param size The number of adjacent labels
 * @param edges The edges that are part of the image
 * @param equivalencies The labels that are equivalent to each other
 * 
 * @return The label of the edge point that was added
 * 
 * Updates edges with the new pixel as appropriate
 */
inline int NWayEquivalenceAdd(Image &image,  // NOLINTBEGIN
                               uint64_t index,
                               int &L,
                               int adjacentLabels[4],
                               int size,
                               std::unordered_map<int, Edge> &edges,
                               std::unordered_map<int, int> &equivalencies) {  // NOLINTEND
    Vec2 pixel = {DECIMAL(index % image.width), DECIMAL(index / image.width)};
    if (size == 0) {
        // No adjacent labels
        edges.insert({++L, {{pixel}, pixel, pixel}});
        return L;
    } else if (size == 1) {
        // One adjacent label
        UpdateEdge(edges[adjacentLabels[0]], pixel);
        return adjacentLabels[0];
    } else if (size == 2) {  // Added for optimization
        if (adjacentLabels[0] < adjacentLabels[1]) {
            // Two adjacent labels, first is smaller
            UpdateEdge(edges[adjacentLabels[0]], pixel);
            if (equivalencies.find(adjacentLabels[1]) == equivalencies.end()) {
                equivalencies.insert({adjacentLabels[1], adjacentLabels[0]});
            } else {
                equivalencies[adjacentLabels[1]] = std::min(equivalencies[adjacentLabels[1]], adjacentLabels[0]);
            }
            return adjacentLabels[0];
        }

        // Two adjacent labels, second is smaller
        UpdateEdge(edges[adjacentLabels[1]], pixel);
        if (equivalencies.find(adjacentLabels[0]) == equivalencies.end()) {
            equivalencies.insert({adjacentLabels[0], adjacentLabels[1]});
        } else {
            equivalencies[adjacentLabels[0]] = std::min(equivalencies[adjacentLabels[0]], adjacentLabels[1]);
        }
        return adjacentLabels[1];
    }
    int minLabel = adjacentLabels[0];
    for (int i = 1; i < size; i++) {
        if (adjacentLabels[i] < minLabel) {
            minLabel = adjacentLabels[i];
        }
    }
    UpdateEdge(edges[minLabel], pixel);
    for (int i = 0; i < size; i++) {
        if (adjacentLabels[i] != minLabel) {
            if (equivalencies.find(adjacentLabels[i]) == equivalencies.end()) {
                equivalencies.insert({adjacentLabels[i], minLabel});
            } else {
                equivalencies[adjacentLabels[i]] = std::min(equivalencies[adjacentLabels[i]], minLabel);
            }
        }
    }
    return minLabel;
}

Edges ConnectedComponentsAlgorithm(Image &image, bool (*Criteria)(uint64_t, Image &)) {
    // Step 0: Setup the Problem
    std::unordered_map<int, Edge> edges;
    std::unordered_map<int, int> equivalencies;
    std::unordered_map<uint64_t, int> edgePoints;

    int L = -1;
    int adjacentLabels[4];
    int size = 0;

    // Step 1: Iterate through the image, forming primary groups of
    // edges, taking note of equivalent edges

    // Step 1a: Tackle the First Pixel
    if (Criteria(0, image)) {
        edges.insert({++L, {{{0, 0}}, {0, 0}, {0, 0}}});
        edgePoints.insert({0, L});
    }

    for (uint64_t i = 1; i < static_cast<uint64_t>(image.width * image.height); i++) {
        // Step 1b: Check if the pixel is an edge point
        if (!Criteria(i, image)) {
            continue;
        }

        // Step 1c: Figure out all adjacent labels
        if (i / image.width == 0) {
            // Top Row (1 other pixel)
            if (auto left = edgePoints.find(i - 1); left != edgePoints.end()) {
                adjacentLabels[size++] = left->second;
            }
        } else if (i % image.width == 0) {
            // Left Column (2 other pixels)
            if (auto top = edgePoints.find(i - image.width); top != edgePoints.end()) {
                adjacentLabels[size++] = top->second;
            }
            if (auto topRight = edgePoints.find(i - image.width + 1); topRight != edgePoints.end()) {
                if (!LabelPresent(topRight->second, adjacentLabels, size)) {
                    adjacentLabels[size++] = topRight->second;
                }
            }
        } else if ((i + 1) % image.width == 0) {
            // Right Column (3 other pixels)
            if (auto left = edgePoints.find(i - 1); left != edgePoints.end()) {
                adjacentLabels[size++] = left->second;
            }
            if (auto topLeft = edgePoints.find(i - image.width - 1); topLeft != edgePoints.end()) {
                if (!LabelPresent(topLeft->second, adjacentLabels, size)) {
                    adjacentLabels[size++] = topLeft->second;
                }
            }
            if (auto top = edgePoints.find(i - image.width); top != edgePoints.end()) {
                if (!LabelPresent(top->second, adjacentLabels, size)) {
                    adjacentLabels[size++] = top->second;
                }
            }
        } else {
            // All others pixels (4 other pixels)
            if (auto left = edgePoints.find(i - 1); left != edgePoints.end()) {
                adjacentLabels[size++] = left->second;
            }
            if (auto topLeft = edgePoints.find(i - image.width - 1); topLeft != edgePoints.end()) {
                if (!LabelPresent(topLeft->second, adjacentLabels, size)) {
                    adjacentLabels[size++] = topLeft->second;
                }
            }
            if (auto top = edgePoints.find(i - image.width); top != edgePoints.end()) {
                if (!LabelPresent(top->second, adjacentLabels, size)) {
                    adjacentLabels[size++] = top->second;
                }
            }
            if (auto topRight = edgePoints.find(i - image.width + 1); topRight != edgePoints.end()) {
                if (!LabelPresent(topRight->second, adjacentLabels, size)) {
                    adjacentLabels[size++] = topRight->second;
                }
            }
        }

        // Step 1d: Add the pixel to the appropriate edge and prepare for the next iteration
        edgePoints.insert({i, NWayEquivalenceAdd(image, i, L, adjacentLabels, size, edges, equivalencies)});
        size = 0;
    }

    // Step 2: Now we need to merge the equivalent edges. We merge the higher
    // label into the lower label, and update the lowest and highest points,
    // and then get rid of the higher label's edge data. We iterate from highest to lowest
    for (int i = L; i >= 0; i--) {
        auto it = equivalencies.find(i);
        if (it == equivalencies.end()) continue;

        // Guarenteed to be the lowest label
        int lowestLabel = it->second;

        // Merge the edges
        auto edgeIt = edges.find(i);
        // edgeIt is guarenteed to exist, so we do not perform a check here
        auto &edgeToMerge = edgeIt->second;
        auto &lowestEdge = edges[lowestLabel];
        lowestEdge.points.insert(lowestEdge.points.end(), edgeToMerge.points.begin(), edgeToMerge.points.end());
        if (edgeToMerge.upperLeft.x < lowestEdge.upperLeft.x) lowestEdge.upperLeft.x = edgeToMerge.upperLeft.x;
        if (edgeToMerge.lowerRight.x > lowestEdge.lowerRight.x) lowestEdge.lowerRight.x = edgeToMerge.lowerRight.x;
        // We skip this statement, because its impossible (a higher edge is level or lower than a lower edge):
        // if (edgeToMerge.upperLeft.y < lowestEdge.upperLeft.y) lowestEdge.upperLeft.y = edgeToMerge.upperLeft.y;
        if (edgeToMerge.lowerRight.y > lowestEdge.lowerRight.y) lowestEdge.lowerRight.y = edgeToMerge.lowerRight.y;

        edges.erase(edgeIt);
    }

    // Step 3: Return the edges
    Edges result;
    for (const auto &[label, edge] : edges) result.push_back(edge);

    return result;
}

}  // namespace found
