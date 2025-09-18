#include "distance/edge.hpp"

#include <unordered_map>
#include <algorithm>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_set>
#include <utility>

#include "common/style.hpp"
#include "common/decimal.hpp"
#include "common/spatial/attitude-utils.hpp"

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
        if ((component.upperLeft.x < this->borderLength_ ||
            component.upperLeft.y < this->borderLength_ ||
            component.lowerRight.x >= image.width - this->borderLength_ ||
            component.lowerRight.y >= image.height - this->borderLength_)) {
            if (!space || component.points.size() > space->points.size())
            space = &component;
        }
    }
    if (space == nullptr || space->points.size() == imageSize) return Points();
    std::unordered_set<uint64_t> &points = space->points;

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
            start = static_cast<uint64_t>(space->lowerRight.y * image.width + space->upperLeft.x);
            offset = -this->offset_;
            edge_condition = image.height - 1;
        } else {
            // Iterate down, and start at the top left corner
            update = image.width;
            start = static_cast<uint64_t>(space->upperLeft.y * image.width + space->upperLeft.x);
            offset = this->offset_;
            edge_condition = 0;
        }
        // Step 2c: Get all edge points along the edge, identifying the edge
        // as the first point that is found inside space
        for (int col = space->upperLeft.x; col <= space->lowerRight.x; col++) {
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
            start = static_cast<uint64_t>(space->upperLeft.y * image.width + space->lowerRight.x);
            offset = -this->offset_;
            edge_condition = image.width - 1;
        } else {
            // Iterate right, and start at the top left corner
            update = 1;
            start = static_cast<uint64_t>(space->upperLeft.y * image.width + space->upperLeft.x);
            offset = this->offset_;
            edge_condition = 0;
        }
        // Step 2c: Get all edge points along the edge, identifying the edge
        // as the first point that is found inside space
        for (int row = space->upperLeft.y; row <= space->lowerRight.y; row++) {
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

////// Convolutional Edge Detection Algorithm //////

Points ConvolutionEdgeDetectionAlgorithm::Run(const Image &image) {
    // Step 0: Define Common Variables
    int64_t image_size = static_cast<int64_t>(image.width) * image.height * image.channels;
    // Step 1: Convolve the image with the mask
    Tensor tensor = ConvolveWithMask(image);
    // Step 2: Identify the edge using the criterion
    Points result;
    for (int64_t k = 0; k < image_size; k += image.channels) {
        if (ApplyCriterion(k, tensor, image)) {
            result.push_back({DECIMAL(k / image.channels % image.width), DECIMAL(k / image.channels / image.width)});
        }
    }
    // TODO: Step 3: Post-process the edge points to remove noise and ensure polar ordering
    // Step 4: Return the points
    return result;
}

Tensor ConvolutionEdgeDetectionAlgorithm::ConvolveWithMask(const Image &image) {
    // Step -1: check if image and mask channels match
    if (image.channels != mask_.channels) {
        throw std::invalid_argument("Image and mask channels do not match");
    }
    // Step 0: setup basic constants
    auto result = std::make_unique<decimal[]>(image.width * image.height * image.channels);
    int64_t center = (static_cast<int64_t>(mask_.centerHeight) * mask_.width + mask_.centerWidth);
    int64_t image_size = static_cast<int64_t>(image.width) * image.height * image.channels;

    // Step 2: perform the convolution
    // Step 2a: loop through the image channels
    for (int channel = 0; channel < image.channels; ++channel) {
        // Step 2b: loop through the image pixels
        for (int64_t k = channel; k < image_size; k += image.channels) {
            // use double for precision then cast back to float once kernel addition is done TODO: switch to decimal
            decimal c = 0;
            // Step 2c: apply the mask to the image
            for (int i = -mask_.centerHeight; i <= mask_.height - mask_.centerHeight - 1; ++i) {
                int row = (k / image.channels) / image.width - i;
                // "valid" padding (ignores pixels outside the image)
                if (row < 0 || row > image.height - 1) {
                    continue;
                }
                for (int j = -mask_.centerWidth; j <= mask_.width - mask_.centerWidth - 1; ++j) {
                    int col = ((k / image.channels) % image.width) - j;
                    // "valid" padding (ignores pixels outside the image)
                    if (col < 0 || col > image.width - 1) {
                        continue;
                    }
                    c += static_cast<decimal>(mask_.data[(center + i * mask_.width + j) * mask_.channels + channel]) *
                         static_cast<decimal>(image.image[(static_cast<int64_t>(row) * image.width + col) *
                                                         image.channels + channel]);
                }
            }
            result[k] = static_cast<float>(c);
        }
    }
    // Apply the mask to the image
    return Tensor(image.width, image.height, image.channels, std::move(result));
}

bool ConvolutionEdgeDetectionAlgorithm::ApplyCriterion(int64_t index, const Tensor &tensor, const Image &image) {
    std::vector<bool> channelIsEdge(image.channels, false);
    // Apply the box based outlier criterion to each channel
    for (int i = 0; i < image.channels; i++) {
        // Only apply the criterion if the tensor is above the threshold
        if (DECIMAL_ABS(tensor.tensor[index]) > threshold_) {
            channelIsEdge[i] = BoxBasedOutlierCriterion(index + i, tensor, image);
        } else {
            channelIsEdge[i] = false;
        }
    }
    return CombineChannelCriterion(channelIsEdge);
}

bool ConvolutionEdgeDetectionAlgorithm::BoxBasedOutlierCriterion(int64_t index,
                                                                 const Tensor &tensor,
                                                                 const Image &image) {
    // Step 1: find the inertia tensor of the box around index
    int boxCenter = boxBasedMaskSize_ / 2;
    // Only need to caluclate 3 values due to symmetry and evaluating a 2D-plane
    decimal inertiaTensor[3] = {0, 0, 0};
    for (int i = -boxCenter; i <= boxBasedMaskSize_ - boxCenter - 1; ++i) {
        int row = (index / tensor.channels) / tensor.width + i;
        // "valid" padding (ignores pixels outside the image)
        if (row < 0 || row > tensor.height - 1) {
            continue;
        }
        for (int j = -boxCenter; j <= boxBasedMaskSize_ - boxCenter - 1; ++j) {
            int col = ((index / tensor.channels) % tensor.width) + j;
            // "valid" padding (ignores pixels outside the image)
            if (col < 0 || col > tensor.width - 1) {
                continue;
            }
            inertiaTensor[0] += tensor.tensor[(static_cast<int64_t>(row) * tensor.width + col) *
                                                    tensor.channels] * (i * i);
            inertiaTensor[1] += tensor.tensor[(static_cast<int64_t>(row) * tensor.width + col) *
                                                    tensor.channels] * (j * j);
            inertiaTensor[2] -= tensor.tensor[(static_cast<int64_t>(row) * tensor.width + col) *
                                                    tensor.channels] * (i * j);
        }
    }

    // Step 2: find the eigenvalues and eigenvector with lowest eigenvalue of the inertia tensor
    decimal discrim = DECIMAL_SQRT((DECIMAL_POW((inertiaTensor[0]-inertiaTensor[1]), 2) +
                                    4*DECIMAL_POW(inertiaTensor[2], 2)));
    decimal lambda1 = (inertiaTensor[0] + inertiaTensor[1] + discrim) / 2;
    decimal lambda2 = (inertiaTensor[0] + inertiaTensor[1] - discrim) / 2;
    // Step 2a: check the ratio of the eigenvalues
    if ((DECIMAL_ZERO(lambda1) && DECIMAL_ZERO(lambda2)) || lambda2 / lambda1 > eigenValueRatio_) return false;
    // Step 2b: find the eigenvector with the lowest eigenvalue
    Vec2 edgeDirection = Vec2{inertiaTensor[2], lambda2 - inertiaTensor[0]}.Normalize();
    // Step 2c: deal with perfect horizontal line case (vertical line case works out)
    if (DECIMAL_ZERO(lambda2) && DECIMAL_ZERO(inertiaTensor[0])) edgeDirection = Vec2{1, 0};

    // Step 3a: Setup constants
    decimal radius = boxBasedMaskSize_ / DECIMAL_MAX(edgeDirection.x, edgeDirection.y) / 2;
    int row = (index / tensor.channels) / tensor.width;
    int col = ((index / tensor.channels) % tensor.width);

    // Step 3b: Test gradient ratio at the ends of the edge direction
    int xCoordBox = DECIMAL_CEIL(edgeDirection.x * radius);
    int yCoordBox = DECIMAL_CEIL(edgeDirection.y * radius);
    // pixels to check fall within the image
    if (0 < row - yCoordBox && row - yCoordBox < tensor.height - 1 &&
        0 < row + yCoordBox && row + yCoordBox < tensor.height - 1 &&
        0 < col - xCoordBox && col - xCoordBox < tensor.width - 1 &&
        0 < col + xCoordBox && col + xCoordBox < tensor.width + 1) {
        // gradient at the two ends of the edge direction
        decimal edgeGradient1 = tensor.tensor[(static_cast<int64_t>(row + yCoordBox) *
                                tensor.width + col + xCoordBox) * tensor.channels];
        decimal edgeGradient2 = tensor.tensor[(static_cast<int64_t>(row - yCoordBox) *
                                tensor.width + col - xCoordBox) * tensor.channels];
        if (DECIMAL_MIN(edgeGradient1, edgeGradient2) /
            DECIMAL_MAX(edgeGradient1, edgeGradient2) < edgeGradientRatio_) return false;
    }

    // Step 3c: test graytone values with greatest distance (projection) to the edge
    Vec2 orthogonalDirection = edgeDirection.Orthogonal();
    int xCoordBoxOrtho = DECIMAL_CEIL(orthogonalDirection.x * radius);
    int yCoordBoxOrtho = DECIMAL_CEIL(orthogonalDirection.y * radius);
    // pixels to check fall within the image
    if (0 < row - yCoordBoxOrtho && row - yCoordBoxOrtho < tensor.height - 1 &&
        0 < row + yCoordBoxOrtho && row + yCoordBoxOrtho < tensor.height - 1 &&
        0 < col - xCoordBoxOrtho && col - xCoordBoxOrtho < tensor.width - 1 &&
        0 < col + xCoordBoxOrtho && col + xCoordBoxOrtho < tensor.width + 1) {
        // graytone farthest from the edge hopefully one is plannet and one space
        decimal grayTone1 = DECIMAL(image.image[(static_cast<int64_t>(row + yCoordBoxOrtho)
                                    * image.width + col + xCoordBoxOrtho) * image.channels]);
        decimal grayTone2 = DECIMAL(image.image[(static_cast<int64_t>(row - yCoordBoxOrtho)
                                    * image.width + col - xCoordBoxOrtho) * image.channels]);
        if (grayTone1 == grayTone2 && DECIMAL_MIN(grayTone1, grayTone2) /
            DECIMAL_MAX(grayTone1, grayTone2) > spacePlanetGraytoneRatio_) return false;
    }

    // If no criteria fail, return true
    return true;
}

bool ConvolutionEdgeDetectionAlgorithm::CombineChannelCriterion(const std::vector<bool> &channelIsEdge) {
    // Step 1: Count the number of channels that are edges
    size_t edgeCount = 0;
    for (bool isEdge : channelIsEdge) {
        if (isEdge) edgeCount++;
    }
    // Step 2: Determine if the pixel is an edge based on the ratio
    return edgeCount >= channelCriterionRatio_ * channelIsEdge.size();
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
 * @param pixel The pixel to add
 * 
 * @pre Must be called in order of increasing index
 */
inline void UpdateComponent(Component &component, uint64_t index, Vec2 &pixel) {
    component.points.insert(index);
    if (component.upperLeft.x > pixel.x) component.upperLeft.x = pixel.x;
    else if (component.lowerRight.x < pixel.x) component.lowerRight.x = pixel.x;
    // We skip this statement, since its impossible:
    // if (component.upperLeft.y > pixel.y) component.upperLeft.y = pixel.y;
    if (component.lowerRight.y < pixel.y) component.lowerRight.y = pixel.y;
}

/**
 * Adds a pixel to some component, creating a new component if necessary
 * 
 * @param image The image to which the pixel belongs
 * @param index The index of the pixel
 * @param L The current label
 * @param adjacentLabels The labels of the adjacent components
 * @param size The number of adjacent labels
 * @param components The components that are part of the image
 * @param equivalencies The labels that are equivalent to each other
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
                              std::unordered_map<int, Component> &components,
                              std::unordered_map<int, int> &equivalencies) {
    Vec2 pixel = {DECIMAL(index % image.width), DECIMAL(index / image.width)};
    if (size == 0) {
        // No adjacent labels
        components.insert({++L, {{index}, pixel, pixel}});
        return L;
    } else if (size == 1) {
        // One adjacent label
        UpdateComponent(components[adjacentLabels[0]], index, pixel);
        return adjacentLabels[0];
    } else if (size == 2) {  // Added for optimization
        if (adjacentLabels[0] < adjacentLabels[1]) {
            // Two adjacent labels, first is smaller
            UpdateComponent(components[adjacentLabels[0]], index, pixel);
            if (equivalencies.find(adjacentLabels[1]) == equivalencies.end()) {
                equivalencies.try_emplace(adjacentLabels[1], adjacentLabels[0]);
            } else {
                equivalencies[adjacentLabels[1]] = std::min(equivalencies[adjacentLabels[1]], adjacentLabels[0]);
            }
            return adjacentLabels[0];
        }

        // Two adjacent labels, second is smaller
        UpdateComponent(components[adjacentLabels[1]], index, pixel);
        if (equivalencies.find(adjacentLabels[0]) == equivalencies.end()) {
            equivalencies.try_emplace(adjacentLabels[0], adjacentLabels[1]);
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
    UpdateComponent(components[minLabel], index, pixel);
    for (int i = 0; i < size; i++) {
        if (adjacentLabels[i] != minLabel) {
            if (equivalencies.find(adjacentLabels[i]) == equivalencies.end()) {
                equivalencies.try_emplace(adjacentLabels[i], minLabel);
            } else {
                equivalencies[adjacentLabels[i]] = std::min(equivalencies[adjacentLabels[i]], minLabel);
            }
        }
    }
    return minLabel;
}

Components ConnectedComponentsAlgorithm(const Image &image, std::function<bool(uint64_t, const Image &)> Criteria) {
    // Step 0: Setup the Problem
    std::unordered_map<int, Component> components;
    std::unordered_map<int, int> equivalencies;
    std::unique_ptr<int[]> componentPoints(new int[image.width * image.height]{});  // Faster than using a hashset

    int L = 0;
    int adjacentLabels[4];
    int size = 0;

    // Step 1: Iterate through the image, forming primary groups of
    // components, taking note of equivalent components

    // Step 1a: Tackle the First Pixel
    if (Criteria(0, image)) {
        components.insert({++L, {{0}, {0, 0}, {0, 0}}});
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
        componentPoints[i] = NWayEquivalenceAdd(image, i, L, adjacentLabels, size, components, equivalencies);
        size = 0;
    }

    // Step 2: Now we need to merge the equivalent components. We merge the higher
    // label into the lower label, and update the lowest and highest points,
    // and then get rid of the higher label's component data. We iterate from highest to lowest
    for (int i = L; i >= 0; i--) {
        auto it = equivalencies.find(i);
        if (it == equivalencies.end()) continue;

        // Guarenteed to be the lowest label
        int lowestLabel = it->second;

        // Merge the components
        auto compIt = components.find(i);
        // compIt is guarenteed to exist, so we do not perform a check here
        auto &compToMerge = compIt->second;
        auto &lowestComp = components[lowestLabel];
        lowestComp.points.insert(compToMerge.points.begin(), compToMerge.points.end());
        if (compToMerge.upperLeft.x < lowestComp.upperLeft.x) lowestComp.upperLeft.x = compToMerge.upperLeft.x;
        if (compToMerge.lowerRight.x > lowestComp.lowerRight.x) lowestComp.lowerRight.x = compToMerge.lowerRight.x;
        // We skip this statement, because its impossible (a higher component is level or lower than a lower component):
        // if (compToMerge.upperLeft.y < lowestComp.upperLeft.y) lowestComp.upperLeft.y = compToMerge.upperLeft.y;
        if (compToMerge.lowerRight.y > lowestComp.lowerRight.y) lowestComp.lowerRight.y = compToMerge.lowerRight.y;

        components.erase(compIt);
    }

    // Step 3: Return the components
    Components result;
    for (const auto &[label, component] : components) result.push_back(component);

    return result;
}

}  // namespace found
