#include "distance/edge.hpp"

#include <unordered_map>
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
        if ((component.upperLeft.x() < this->borderLength_ ||
            component.upperLeft.y() < this->borderLength_ ||
            component.lowerRight.x() >= image.width - this->borderLength_ ||
            component.lowerRight.y() >= image.height - this->borderLength_)) {
            if (!space || component.points.size() > space->points.size())
            space = &component;
        }
    }
    if (space == nullptr || space->points.size() == imageSize) return Points();
    std::unordered_set<uint64_t> &points = space->points;

    // Step 2: Identify the edge as the edge of space

    // Step 2a: Figure out the centroids of space and the planet
    Vec2 planetCentroid(0, 0);
    Vec2 spaceCentroid(0, 0);
    int64_t planetSize = 0;
    int64_t spaceSize = 0;
    for (uint64_t i = 0; i < imageSize; i++) {
        if (points.find(i) == points.end()) {
            planetCentroid.x() += i % image.width;
            planetCentroid.y() += i / image.width;
            planetSize++;
        } else {
            spaceCentroid.x() += i % image.width;
            spaceCentroid.y() += i / image.width;
            spaceSize++;
        }
    }
    planetCentroid.x() /= planetSize;
    planetCentroid.y() /= planetSize;
    spaceCentroid.x() /= spaceSize;
    spaceCentroid.y() /= spaceSize;

    Vec2 itrDirection = spaceCentroid - planetCentroid;

    // Step 2b: Figure out how to iterate through the image,
    // iterating from the planet into space
    Points result;
    if (std::abs(itrDirection.y()) > std::abs(itrDirection.x())) {
        // Determine which direction we want to iterate,
        // (we want to iterate into the space)
        uint64_t update;
        uint64_t start;
        decimal offset;
        uint64_t edge_condition;
        if (itrDirection.y() < 0) {
            // Iterate up, and start at the bottom left corner
            update = -image.width;
            start = static_cast<uint64_t>(space->lowerRight.y() * image.width + space->upperLeft.x());
            offset = -this->offset_;
            edge_condition = image.height - 1;
        } else {
            // Iterate down, and start at the top left corner
            update = image.width;
            start = static_cast<uint64_t>(space->upperLeft.y() * image.width + space->upperLeft.x());
            offset = this->offset_;
            edge_condition = 0;
        }
        // Step 2c: Get all edge points along the edge, identifying the edge
        // as the first point that is found inside space
        for (int col = space->upperLeft.x(); col <= space->lowerRight.x(); col++) {
            // because index is uint64_t, going below zero will overflow, and it will indeed be past the dimensions
            uint64_t index = start;
            while (points.find(index) == points.end()) index += update;
            if (index / image.width != edge_condition) {
                index -= update;
                result.push_back(Vec2(DECIMAL(index % image.width),
                                    DECIMAL(index / image.width) - offset));
            }
            start++;
        }
    } else {
        // Determine which direction we want to iterate
        uint64_t update;
        uint64_t start;
        decimal offset;
        uint64_t edge_condition;
        if (itrDirection.x() < 0) {
            // Iterate left, and start at the top right corner
            update = -1;
            start = static_cast<uint64_t>(space->upperLeft.y() * image.width + space->lowerRight.x());
            offset = -this->offset_;
            edge_condition = image.width - 1;
        } else {
            // Iterate right, and start at the top left corner
            update = 1;
            start = static_cast<uint64_t>(space->upperLeft.y() * image.width + space->upperLeft.x());
            offset = this->offset_;
            edge_condition = 0;
        }
        // Step 2c: Get all edge points along the edge, identifying the edge
        // as the first point that is found inside space
        for (int row = space->upperLeft.y(); row <= space->lowerRight.y(); row++) {
            uint64_t index = start;
            while (points.find(index) == points.end()) index += update;
            if (index % image.width != edge_condition) {
                index -= update;
                result.push_back(Vec2(DECIMAL(index % image.width) - offset,
                                    DECIMAL(index / image.width)));
            }
            start += image.width;
        }
    }

    // Step 4: Return the points
    return result;
}


////// Inertial Symmetry Edge Detection Algorithm //////

// Using a start point and a direction of a given ray, finds the point of
// intersection at the edge of the image canvas.
static Vec2 FindImageEdge(int imageWidth, 
                          int imageHeight, 
                          Vec2 start,
                          Vec2 direction) {
    // Remove one from the width and height to ensure that we only get valid
    // indices in the image.
    imageWidth -= 1;
    imageHeight -= 1;

    // Edge case: no direction given.
    if (direction.x == 0 && direction.y == 0)
        return start;

    // Edge case: one component is zero.
    if (direction.x == 0)
        return direction.x < 0 ? Vec2(0, start.y) : Vec2(imageWidth, start.y);
    else if (direction.y == 0)
        return direction.y < 0 ? Vec2(start.x, 0) : Vec2(start.x, imageHeight);

    decimal slope = direction.y / direction.x;
    
    // Intersection with y = 0
    decimal x1 = start.x - start.y / slope;
    if (x1 >= 0 && x1 <= imageWidth && direction.y < 0)
        return Vec2(0, x1);
    
    // Intersection with y = imageHeight
    decimal x2 = start.x - (start.y - imageHeight) / slope;
    if (x2 >= 0 && x2 <= imageWidth && direction.y > 0) {
        return Vec2(imageHeight, x2);
    }

    // Intersection with x = 0
    decimal y1 = start.y - start.x * slope;
    if (y1 >= 0 && y1 <= imageHeight && direction.x < 0) {
        return Vec2(y1, 0);
    }

    // Intersection with x = imageWidth
    decimal y2 = start.y - (start.x - imageWidth) * slope;
    return Vec2(y2, imageWidth);
}

// Using a binary image, give a value in [0,1] using a bilinear sample at the
// given image coordinate.
static decimal SampleImageBilinear(int imageWidth, 
                                   int imageHeight,
                                   std::vector<bool> &pixels, 
                                   Vec2 position) {
    int ix = (int) position.x;
    int iy = (int) position.y;
    
    // Assume black when out of bounds of the image.
    if (ix + 1 >= imageWidth || iy + 1 >= imageHeight)
        return 0;

    // Get a coordinate relative to the pixel in the image
    decimal px = position.x - ix;
    decimal py = position.y - iy;

    // Implicit cast here from bool to decimal
    decimal v0 = pixels[iy * imageWidth + ix];
    decimal v1 = pixels[iy * imageWidth + (ix + 1)];
    decimal v2 = pixels[(iy + 1) * imageWidth + ix];
    decimal v3 = pixels[(iy + 1) * imageWidth + (ix + 1)];
    
    return (DECIMAL(1.0) - py) * 
        (v0 * (DECIMAL(1.0) - px) + v1 * px) + 
        py * (v2 * (DECIMAL(1.0) - px) + v3 * px);
}

Points InertialSymmetryEdgeDetectionAlgorithm::Run(const Image &image) {
    // Step 1: Image processing
    // A std::vector<bool> is used here as C++ allows storing bool vectors as
    // bitsets; this is all the size we need.
    std::vector<bool> pixels(image.width * image.height);

    // First, average the channels into one value, then threshold the values.
    // A max of three channels are averaged, just so we don't include the alpha
    // channel in calculations.
    //
    // To simplify the next step, the "mass" and centroid of the thresholded
    // area is also calculated in this step.
    int averagedChannels = std::min(image.channels, 3);
    int mass = 0;
    Vec2 centroid;
    for (int x = 0; x < image.width; x++) {
        for (int y = 0; y < image.height; y++) {
            int sum = 0;
            for (int k = 0; k < averagedChannels; k++)
                sum += image.image[(y * image.width + x) * image.channels + k];
            int value = sum / averagedChannels;
            pixels[y * image.width + x] = value > grayThreshold_;
            if (value > grayThreshold_) {
                mass++;
                centroid.x += x;
                centroid.y += y;
            }
        }
    }
    centroid = centroid * (DECIMAL(1.0) / DECIMAL(mass));
    
    // Step 2: Calculating inertial properties
    // Calculate the moment and product of inertia of the thresholded area. This
    // allows us to guess the radius and best line of symmetry with some
    // eigenanalysis of its inertia tensor.
    decimal Ix = 0, Iy = 0, Ixy = 0;
    for (int x = 0; x < image.width; x++) {
        for (int y = 0; y < image.height; y++) {
            if (pixels[y * image.width + x]) {
                decimal dx = DECIMAL(x) - centroid.x;
                decimal dy = DECIMAL(y) - centroid.y;
                Ix += dx * dx;
                Iy += dy * dy;
                Ixy -= dx * dy;
            }
        }
    }
    
    // Trace and determinant of the inertia tensor.
    decimal trace = Ix + Iy;
    decimal determinant = Ix * Iy - Ixy * Ixy;
    
    // Eigenvalues of the inertia tensor.
    decimal Lmax = DECIMAL(0.5) * (trace + DECIMAL_SQRT(trace * trace - 4 * determinant));
    decimal Lmin = DECIMAL(0.5) * (trace - DECIMAL_SQRT(trace * trace - 4 * determinant));

    // Eigenvectors of the inertia tensor, and normalizations of each.
    // The eigenvector associated with the larger eigenvalue is the minor axis
    // of the thresholded area.
    Vec2 wmax{ 1, (Lmax - Ix) / Ixy };
    Vec2 wmaxHat = wmax.Normalize();

    // The eigenvector associated with the smaller eigenvalue is the major axis
    // of the thresholded area.
    Vec2 wmin{ 1, (Lmin - Ix) / Ixy };
    Vec2 wminHat = wmin.Normalize();

    // Really rough estimate of the radius of the celestial body in the frame.
    decimal estimatedRadius = 2 * DECIMAL_SQRT(Lmax / mass);
    
    // Offset betwen the lines of correlation. Tries to split the radius evenly
    // with the number of lines.
    Vec2 offset = wminHat * estimatedRadius / lineCount_;

    // Step 3: Placing edge points

    // Two sets of points to choose between with a simple fit.
    Points red;
    Points blue;
    for (int i = -lineCount_; i <= lineCount_; i++) {
        // First, find the lines by getting the points that intersect the image
        // bounds.
        Vec2 start = FindImageEdge(
            image.width, image.height,
            centroid + (offset * i),
            wmaxHat
        );
        Vec2 end = FindImageEdge(
            image.width, image.height,
            centroid + (offset * i),
            -wmaxHat
        );

        decimal length = (end - start).Magnitude();

        // Skip lines that are too short.
        if (length < lineEpsilon_)
            continue;

        // Direction vector of pixel length one.
        Vec2 dir = (end - start) / length;

        // Correlate the points on the line with a mask.
        // Both signals are zero-padded.
        constexpr int maskSize = MASK.size();
        int maxLength = maskSize <= length ? (int) length : maskSize;
        int minLength = maskSize > length ? (int) length : maskSize;

        // Number of points where both signals overlap completely, to minimize
        // boundary problems.
        int validLength = maxLength - minLength + 1;

        // This array is, where f is the samples on the line and g is
        // the mask, equal to (f * g)(-n); importantly, NOT (f * g)(n).
        std::vector<decimal> correlation(validLength);
        decimal maxCorrelation = 0;
        for (int n = 0; n < length; n++) {
            Vec2 point = start + dir * n;
            decimal sample = SampleImageBilinear(
                image.width,
                image.height,
                pixels,
                point
            );
            for (int k = 0; k < validLength; k++) {
                int index = n - k;
                if (index >= 0 && index < maskSize) {
                    correlation[k] += MASK[n - k] * sample;
                    if (correlation[k] > maxCorrelation)
                        maxCorrelation = correlation[k];
                }
                // Else, either signal is zero, and thus nothing is added.
            }
        }

        // Get the first index that passes the halfway point of correlation.
        int i0;
        for (i0 = 0; i0 < validLength; i0++)
            if (correlation[i0] >= maxCorrelation / 2)
                break;
       
        // Get the last index that passes the halfway point of correlation.
        int i1;
        for (i1 = validLength - 1; i1 >= 0; i1--)
            if (correlation[i1] >= maxCorrelation / 2)
                break;

        // Get the points on the line corresponding to the indices.
        Vec2 p0 = start + dir * i0;
        Vec2 p1 = start + dir * i1;

        red.push_back(p0);
        blue.push_back(p1);
    }
    
    return Points();
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
    if (component.upperLeft.x() > pixel.x()) component.upperLeft.x() = pixel.x();
    else if (component.lowerRight.x() < pixel.x()) component.lowerRight.x() = pixel.x();
    // We skip this statement, since its impossible:
    // if (component.upperLeft.y() > pixel.y()) component.upperLeft.y() = pixel.y();
    if (component.lowerRight.y() < pixel.y()) component.lowerRight.y() = pixel.y();
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
        if (compToMerge.upperLeft.x() < lowestComp.upperLeft.x())
            lowestComp.upperLeft.x() = compToMerge.upperLeft.x();
        if (compToMerge.lowerRight.x() > lowestComp.lowerRight.x())
            lowestComp.lowerRight.x() = compToMerge.lowerRight.x();
        // We skip this statement, because its impossible
        // (a higher component is level or lower than a lower component):
        // if (compToMerge.upperLeft.y() < lowestComp.upperLeft.y())
        //     lowestComp.upperLeft.y() = compToMerge.upperLeft.y();
        if (compToMerge.lowerRight.y() > lowestComp.lowerRight.y())
            lowestComp.lowerRight.y() = compToMerge.lowerRight.y();

        components.erase(compIt);
    }

    // Step 3: Return the components
    Components result;
    for (const auto &[label, component] : components) result.push_back(component);

    return result;
}

}  // namespace found
