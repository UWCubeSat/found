#include "distance/edge.hpp"

#include <unordered_map>
#include <memory>

#include "style/style.hpp"
#include "style/decimal.hpp"

namespace found {

/**
 * Updates the edge with the given pixel
 * 
 * @param edge The edge to update
 * @param index The index of the pixel
 * @param pixel The pixel to add
 * 
 * @pre UpdateEdge must be called in order of increasing index
 */
inline void UpdateEdge(Edge &edge, uint64_t index, Vec2 &pixel) {
    edge.points.push_back(pixel);
    edge.highestPoint = index;
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
 * Updates edges with the new pixel as appropriate
 */
inline void NWayEquivalenceAdd(Image &image,  // NOLINTBEGIN
                               uint64_t index,
                               int &L,
                               int *adjacentLabels,
                               int size,
                               std::unordered_map<int, Edge> &edges,
                               std::unordered_map<int, int> &equivalencies) {  // NOLINTEND
    Vec2 pixel = {DECIMAL(index % image.width), DECIMAL(index / image.width)};

    if (size == 0) {
        // No adjacent labels
        edges.insert({++L, {{pixel}, index, index}});
    } else if (size == 1) {
        // One adjacent label
        UpdateEdge(edges[adjacentLabels[0]], index, pixel);
    } else if (size == 2) {  // Added for optimization
        if (adjacentLabels[0] < adjacentLabels[1]) {
            // Two adjacent labels, first is smaller
            UpdateEdge(edges[adjacentLabels[0]], index, pixel);
            equivalencies.insert({adjacentLabels[1], adjacentLabels[0]});
        } else {
            // Two adjacent labels, second is smaller
            UpdateEdge(edges[adjacentLabels[1]], index, pixel);
            equivalencies.insert({adjacentLabels[0], adjacentLabels[1]});
        }
    } else {
        int minLabel = adjacentLabels[0];
        for (int i = 1; i < size; i++) {
            if (adjacentLabels[i] < minLabel) {
                minLabel = adjacentLabels[i];
            }
        }
        UpdateEdge(edges[minLabel], index, pixel);
        for (int i = 0; i < size; i++) {
            if (adjacentLabels[i] != minLabel) {
                equivalencies.insert({adjacentLabels[i], minLabel});
            }
        }
    }
}

Edges ConnectedComponentsAlgorithm(Image &image, bool (*Criteria)(uint64_t, Image &)) {
    // Step 0: Setup the Problem
    std::unordered_map<int, Edge> edges;
    std::unordered_map<int, int> equivalencies;
    std::unordered_map<uint64_t, int> edgePoints;

    int L = 0;
    int adjacentLabels[4];
    int size = 0;

    // Step 1: Iterate through the image, forming primary groups of
    // edges, taking note of equivalent edges

    // Step 1a: Tackle the First Pixel
    if (Criteria(0, image)) {
        edges.insert({++L, {{{0, 0}}, 0, 0}});
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
                adjacentLabels[size++] = topRight->second;
            }
        } else if ((i + 1) % image.width == 0) {
            // Right Column (3 other pixels)
            if (auto left = edgePoints.find(i - 1); left != edgePoints.end()) {
                adjacentLabels[size++] = left->second;
            }
            if (auto topLeft = edgePoints.find(i - image.width - 1); topLeft != edgePoints.end()) {
                adjacentLabels[size++] = topLeft->second;
            }
            if (auto top = edgePoints.find(i - image.width); top != edgePoints.end()) {
                adjacentLabels[size++] = top->second;
            }
        } else {
            // All others pixels (4 other pixels)
            if (auto left = edgePoints.find(i - 1); left != edgePoints.end()) {
                adjacentLabels[size++] = left->second;
            }
            if (auto topLeft = edgePoints.find(i - image.width - 1); topLeft != edgePoints.end()) {
                adjacentLabels[size++] = topLeft->second;
            }
            if (auto top = edgePoints.find(i - image.width); top != edgePoints.end()) {
                adjacentLabels[size++] = top->second;
            }
            if (auto topRight = edgePoints.find(i - image.width + 1); topRight != edgePoints.end()) {
                adjacentLabels[size++] = topRight->second;
            }
        }

        // Step 1d: Add the pixel to the appropriate edge and prepare for the next iteration
        NWayEquivalenceAdd(image, i, L, adjacentLabels, size, edges, equivalencies);
        edgePoints.insert({i, L});
        size = 0;
    }

    // Step 2: Now we need to merge the equivalent edges. We merge the higher
    // label into the lower label, and update the lowest and highest points,
    // and then get rid of the higher label's edge data
    for (auto &equivalence : equivalencies) {
        auto it = edges.find(equivalence.first);

        // TODO(nguy8tri): This check shouldn't fail, so should we fail the algorithm or get rid of the if statement?
        if (it != edges.end()) {
            auto it2 = edges.find(equivalence.second);
            if (it2 != edges.end()) {
                // TODO(anyone): This is a slow operation because we are using a vector instead of a list. Should
                // we consider something else, like a set?
                it2->second.points.insert(it2->second.points.end(),
                                           it->second.points.begin(),
                                           it->second.points.end());
                if (it2->second.lowestPoint > it->second.lowestPoint) {
                    it2->second.lowestPoint = it->second.lowestPoint;
                }
                if (it2->second.highestPoint < it->second.highestPoint) {
                    it2->second.highestPoint = it->second.highestPoint;
                }
                edges.erase(it);
            }
        }
    }

    // Step 3: Return the edges
    Edges result;
    for (const auto &[_, edge] : edges) result.push_back(edge);

    return result;
}

}  // namespace found
