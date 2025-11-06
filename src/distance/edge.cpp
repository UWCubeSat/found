#include "distance/edge.hpp"

#include <algorithm>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_set>

#include "common/style.hpp"
#include "common/decimal.hpp"

namespace found {

////// Simple Edge Detection Algorithm //////

// TODO: Investigate if the use of the DECIMAL(x) cast is taking up too much time

Points SimpleEdgeDetectionAlgorithm::Run(const Image &image) {
    // Step 0: Define Common Variables
    uint64_t imageSize = image.width * image.height;

    // Step 1: Obtain the component that represents space
    Components spaces = ConnectedComponentsAlgorithm(image, [&](uint64_t index, const Image &image) {
        // Average the pixel, then threshold it
        int sum = 0;
        for (int i = 0; i < image.channels; i++) sum += image.image[image.channels * index + i];
        return sum / image.channels < this->threshold_;
    });
    Component *space = nullptr;
    for (auto &component : spaces) {
        // Basically, if the component touches the border, and its the biggest one,
        // we assume it is space
        uint64_t upperLeftX = component.upperLeftIndex % image.width;
        uint64_t upperLeftY = component.upperLeftIndex / image.width;
        uint64_t lowerRightX = component.lowerRightIndex % image.width;
        uint64_t lowerRightY = component.lowerRightIndex / image.width;

        if ((upperLeftX < static_cast<uint64_t>(this->borderLength_) ||
            upperLeftY < static_cast<uint64_t>(this->borderLength_) ||
            lowerRightX >= static_cast<uint64_t>(image.width - this->borderLength_) ||
            lowerRightY >= static_cast<uint64_t>(image.height - this->borderLength_))) {
            if (!space || component.points.size() > space->points.size())
            space = &component;
        }
    }
    if (space == nullptr || space->points.size() == imageSize) return Points();
    std::unordered_set<uint64_t> &points = space->points;

    // Extract x, y coordinates from indices
    uint64_t spaceUpperLeftX = space->upperLeftIndex % image.width;
    uint64_t spaceUpperLeftY = space->upperLeftIndex / image.width;
    uint64_t spaceLowerRightX = space->lowerRightIndex % image.width;
    uint64_t spaceLowerRightY = space->lowerRightIndex / image.width;

    // Step 2: Identify the edge as the edge of space

    // Step 2a: Figure out the centroids of space and the planet
    Vec2 planetCentroid{0, 0};
    Vec2 spaceCentroid{0, 0};
    int64_t planetSize = 0;
    int64_t spaceSize = 0;
    for (uint64_t i = 0; i < imageSize; i++) {
        if (points.find(i) == points.end()) {
            planetCentroid.x += i % image.width;
            planetCentroid.y += i / image.width;
            planetSize++;
        } else {
            spaceCentroid.x += i % image.width;
            spaceCentroid.y += i / image.width;
            spaceSize++;
        }
    }
    planetCentroid.x /= planetSize;
    planetCentroid.y /= planetSize;
    spaceCentroid.x /= spaceSize;
    spaceCentroid.y /= spaceSize;

    Vec2 itrDirection = spaceCentroid - planetCentroid;

    // Step 2b: Figure out how to iterate through the image,
    // iterating from the planet into space
    Points result;
    if (std::abs(itrDirection.y) > std::abs(itrDirection.x)) {
        // Determine which direction we want to iterate,
        // (we want to iterate into the space)
        uint64_t update;
        uint64_t start;
        decimal offset;
        uint64_t edge_condition;
        if (itrDirection.y < 0) {
            // Iterate up, and start at the bottom left corner
            update = -image.width;
            start = spaceLowerRightY * image.width + spaceUpperLeftX;
            offset = -this->offset_;
            edge_condition = image.height - 1;
        } else {
            // Iterate down, and start at the top left corner
            update = image.width;
            start = spaceUpperLeftY * image.width + spaceUpperLeftX;
            offset = this->offset_;
            edge_condition = 0;
        }
        // Step 2c: Get all edge points along the edge, identifying the edge
        // as the first point that is found inside space
        for (uint64_t col = spaceUpperLeftX; col <= spaceLowerRightX; col++) {
            // because index is uint64_t, going below zero will overflow, and it will indeed be past the dimensions
            uint64_t index = start;
            while (points.find(index) == points.end()) index += update;
            if (index / image.width != edge_condition) {
                index -= update;
                result.push_back({DECIMAL(index % image.width),
                                    DECIMAL(index / image.width) - offset});
            }
            start++;
        }
    } else {
        // Determine which direction we want to iterate
        uint64_t update;
        uint64_t start;
        decimal offset;
        uint64_t edge_condition;
        if (itrDirection.x < 0) {
            // Iterate left, and start at the top right corner
            update = -1;
            start = spaceUpperLeftY * image.width + spaceLowerRightX;
            offset = -this->offset_;
            edge_condition = image.width - 1;
        } else {
            // Iterate right, and start at the top left corner
            update = 1;
            start = spaceUpperLeftY * image.width + spaceUpperLeftX;
            offset = this->offset_;
            edge_condition = 0;
        }
        // Step 2c: Get all edge points along the edge, identifying the edge
        // as the first point that is found inside space
        for (uint64_t row = spaceUpperLeftY; row <= spaceLowerRightY; row++) {
            uint64_t index = start;
            while (points.find(index) == points.end()) index += update;
            if (index % image.width != edge_condition) {
                index -= update;
                result.push_back({DECIMAL(index % image.width) - offset,
                                    DECIMAL(index / image.width)});
            }
            start += image.width;
        }
    }

    // Step 4: Return the points
    return result;
}

////// Connected Components Algorithm //////

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
 * Updates the component with the given pixel
 * 
 * @param component The component to update
 * @param index The index to add
 * @param imageWidth The width of the image (for computing x, y coordinates)
 * 
 * @pre Must be called in order of increasing index
 */
inline void UpdateComponent(Component &component, uint64_t index, int imageWidth) {
    component.points.insert(index);

    // Get x, y coordinates from index
    uint64_t x = index % imageWidth;
    uint64_t y = index / imageWidth;

    // Get current bounding box coordinates
    uint64_t upperLeftIdx = component.upperLeftIndex;
    uint64_t lowerRightIdx = component.lowerRightIndex;

    uint64_t upperLeftX = upperLeftIdx % imageWidth;
    uint64_t upperLeftY = upperLeftIdx / imageWidth;

    uint64_t lowerRightX = lowerRightIdx % imageWidth;
    uint64_t lowerRightY = lowerRightIdx / imageWidth;

    // Update bounding box - track min and max independently
    bool needUpdate = false;

    // Update minX
    if (x < upperLeftX) {
        upperLeftX = x;
        needUpdate = true;
    }
    // Update minY
    if (y < upperLeftY) {
        upperLeftY = y;
        needUpdate = true;
    }
    if (needUpdate) {
        component.upperLeftIndex = upperLeftY * imageWidth + upperLeftX;
    }

    // Update maxX and maxY for lowerRight
    needUpdate = false;
    if (x > lowerRightX) {
        lowerRightX = x;
        needUpdate = true;
    }
    if (y > lowerRightY) {
        lowerRightY = y;
        needUpdate = true;
    }
    if (needUpdate) {
        component.lowerRightIndex = lowerRightY * imageWidth + lowerRightX;
    }
}

/**
 * Adds a pixel to some component, creating a new component if necessary
 * 
 * @param image The image to which the pixel belongs
 * @param index The index of the pixel
 * @param L The current label
 * @param adjacentLabels The labels of the adjacent components
 * @param size The number of adjacent labels
 * @param components The components that are part of the image (array)
 * @param equivalencies The labels that are equivalent to each other (array)
 * @param componentExists Track which components exist (array)
 * 
 * @return The label of the component point that was added
 * 
 * Updates components with the new pixel as appropriate
 */
inline int NWayEquivalenceAdd(const Image &image,
                              uint64_t index,
                              int &L,
                              int adjacentLabels[4],
                              int size,
                              std::vector<Component> &components,
                              std::vector<int> &equivalencies,
                              std::vector<bool> &componentExists) {
    if (size == 0) {
        // No adjacent labels
        L++;
        components[L] = {{index}, index, index};  // upperLeftIndex and lowerRightIndex both start at index
        componentExists[L] = true;
        return L;
    } else if (size == 1) {
        // One adjacent label
        UpdateComponent(components[adjacentLabels[0]], index, image.width);
        return adjacentLabels[0];
    } else if (size == 2) {  // Added for optimization
        if (adjacentLabels[0] < adjacentLabels[1]) {
            // Two adjacent labels, first is smaller
            UpdateComponent(components[adjacentLabels[0]], index, image.width);
            if (equivalencies[adjacentLabels[1]] == 0) {
                equivalencies[adjacentLabels[1]] = adjacentLabels[0];
            } else {
                equivalencies[adjacentLabels[1]] = std::min(equivalencies[adjacentLabels[1]], adjacentLabels[0]);
            }
            return adjacentLabels[0];
        }

        // Two adjacent labels, second is smaller
        UpdateComponent(components[adjacentLabels[1]], index, image.width);
        if (equivalencies[adjacentLabels[0]] == 0) {
            equivalencies[adjacentLabels[0]] = adjacentLabels[1];
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
    UpdateComponent(components[minLabel], index, image.width);
    for (int i = 0; i < size; i++) {
        if (adjacentLabels[i] != minLabel) {
            if (equivalencies[adjacentLabels[i]] == 0) {
                equivalencies[adjacentLabels[i]] = minLabel;
            } else {
                equivalencies[adjacentLabels[i]] = std::min(equivalencies[adjacentLabels[i]], minLabel);
            }
        }
    }
    return minLabel;
}

Components ConnectedComponentsAlgorithm(const Image &image, std::function<bool(uint64_t, const Image &)> Criteria) {
    // Step 0: Setup the Problem
    // Early return for empty images (width or height = 0)
    if (image.width == 0 || image.height == 0) {
        return Components();
    }
    if (image.width < 0 || image.height < 0) {
        throw std::invalid_argument("Image dimensions must be positive");
    }
    if (image.width > 100000 || image.height > 100000) {
        throw std::invalid_argument("Image dimensions too large");
    }
    // Calculate maximum capacity: ⌈wh/4⌉ = (wh + 3) / 4
    uint64_t maxComponents = (static_cast<uint64_t>(image.width) * image.height + 3) / 4;
    std::vector<Component> components(maxComponents + 1);  // +1 because labels start at 1
    std::vector<int> equivalencies(maxComponents + 1, 0);
    std::vector<bool> componentExists(maxComponents + 1, false);
    std::unique_ptr<int[]> componentPoints(new int[image.width * image.height]{});

    int L = 0;
    int adjacentLabels[4];
    int size = 0;

    // Step 1: Iterate through the image, forming primary groups of
    // components, taking note of equivalent components

    // Step 1a: Tackle the First Pixel
    if (Criteria(0, image)) {
        L++;
        components[L] = {{0}, 0, 0};  // upperLeftIndex and lowerRightIndex both start at 0
        componentExists[L] = true;
        componentPoints[0] = L;
    }

    uint64_t imageSize = static_cast<uint64_t>(image.width * image.height);
    for (uint64_t i = 1; i < imageSize; i++) {
        // Step 1b: Check if the pixel is an component point
        if (!Criteria(i, image)) {
            continue;
        }

        // Step 1c: Figure out all adjacent labels
        if (i / image.width == 0) {
            // Top Row (1 other pixel)
            if (auto left = componentPoints[i - 1]; left != 0) {
                adjacentLabels[size++] = left;
            }
        } else if (i % image.width == 0) {
            // Left Column (2 other pixels)
            if (auto top = componentPoints[i - image.width]; top != 0) {
                adjacentLabels[size++] = top;
            }
            if (auto topRight = componentPoints[i - image.width + 1]; topRight != 0) {
                if (!LabelPresent(topRight, adjacentLabels, size)) {
                    adjacentLabels[size++] = topRight;
                }
            }
        } else if ((i + 1) % image.width == 0) {
            // Right Column (3 other pixels)
            if (auto left = componentPoints[i - 1]; left != 0) {
                adjacentLabels[size++] = left;
            }
            if (auto topLeft = componentPoints[i - image.width - 1]; topLeft != 0) {
                if (!LabelPresent(topLeft, adjacentLabels, size)) {
                    adjacentLabels[size++] = topLeft;
                }
            }
            if (auto top = componentPoints[i - image.width]; top != 0) {
                if (!LabelPresent(top, adjacentLabels, size)) {
                    adjacentLabels[size++] = top;
                }
            }
        } else {
            // All others pixels (4 other pixels)
            if (auto left = componentPoints[i - 1]; left != 0) {
                adjacentLabels[size++] = left;
            }
            if (auto topLeft = componentPoints[i - image.width - 1]; topLeft != 0) {
                if (!LabelPresent(topLeft, adjacentLabels, size)) {
                    adjacentLabels[size++] = topLeft;
                }
            }
            if (auto top = componentPoints[i - image.width]; top != 0) {
                if (!LabelPresent(top, adjacentLabels, size)) {
                    adjacentLabels[size++] = top;
                }
            }
            if (auto topRight = componentPoints[i - image.width + 1]; topRight != 0) {
                if (!LabelPresent(topRight, adjacentLabels, size)) {
                    adjacentLabels[size++] = topRight;
                }
            }
        }

        // Step 1d: Add the pixel to the appropriate component and prepare for the next iteration
        componentPoints[i] = NWayEquivalenceAdd(image, i, L, adjacentLabels, size,
                                                 components, equivalencies, componentExists);
        size = 0;
    }

    // Step 2: Now we need to merge the equivalent components. We merge the higher
    // label into the lower label, and update the lowest and highest points,
    // and then get rid of the higher label's component data. We iterate from highest to lowest
    for (int i = L; i > 0; i--) {
        if (equivalencies[i] == 0) continue;

        // Guarenteed to be the lowest label
        int lowestLabel = equivalencies[i];

        // Merge the components if this component exists
        if (!componentExists[i]) continue;
        auto &compToMerge = components[i];
        auto &lowestComp = components[lowestLabel];
        lowestComp.points.insert(compToMerge.points.begin(), compToMerge.points.end());

        // Update bounds by taking min/max independently
        uint64_t mergeULIdx = compToMerge.upperLeftIndex;
        uint64_t lowestULIdx = lowestComp.upperLeftIndex;

        uint64_t mergeULX = mergeULIdx % image.width;
        uint64_t mergeULY = mergeULIdx / image.width;

        uint64_t lowestULX = lowestULIdx % image.width;
        uint64_t lowestULY = lowestULIdx / image.width;

        // Update minX and minY
        uint64_t newMinX = std::min(mergeULX, lowestULX);
        uint64_t newMinY = std::min(mergeULY, lowestULY);
        if (newMinX != lowestULX || newMinY != lowestULY) {
            lowestComp.upperLeftIndex = newMinY * image.width + newMinX;
        }

        uint64_t mergeLRIdx = compToMerge.lowerRightIndex;
        uint64_t lowestLRIdx = lowestComp.lowerRightIndex;

        uint64_t mergeLRX = mergeLRIdx % image.width;
        uint64_t mergeLRY = mergeLRIdx / image.width;

        uint64_t lowestLRX = lowestLRIdx % image.width;
        uint64_t lowestLRY = lowestLRIdx / image.width;

        // Update maxX and maxY
        uint64_t newMaxX = std::max(mergeLRX, lowestLRX);
        uint64_t newMaxY = std::max(mergeLRY, lowestLRY);
        if (newMaxX != lowestLRX || newMaxY != lowestLRY) {
            lowestComp.lowerRightIndex = newMaxY * image.width + newMaxX;
        }

        componentExists[i] = false;
    }

    // Step 3: Return the components
    Components result;
    for (int i = 1; i <= L; i++) {
        if (componentExists[i]) {
            result.push_back(components[i]);
        }
    }

    return result;
}

}  // namespace found
