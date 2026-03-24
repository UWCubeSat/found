#include "distance/edge.hpp"

#include <unordered_map>
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#include <functional>
#include <unordered_set>
#include <cmath>

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
    Vec2<> planetCentroid{0, 0};
    Vec2<> spaceCentroid{0, 0};
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

    Vec2<> itrDirection = spaceCentroid - planetCentroid;

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
inline void UpdateComponent(Component &component, uint64_t index, Vec2<> &pixel) {
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
    Vec2<> pixel = {DECIMAL(index % image.width), DECIMAL(index / image.width)};
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

////// Zernike Edge Detection Algorithm //////

std::pair<std::unique_ptr<ComplexNumber[]>, std::unique_ptr<ComplexNumber[]>>
        ZernikeEdgeDetectionAlgorithm::computeZernikeKernels() {
    const std::size_t n = static_cast<std::size_t>(maskSize_ * maskSize_);
    std::unique_ptr<ComplexNumber[]> kernelM11 = std::make_unique<ComplexNumber[]>(n);
    std::unique_ptr<ComplexNumber[]> kernelM20 = std::make_unique<ComplexNumber[]>(n);

    // Normalize window such that the unit circle is inscribed in the window
    decimal pixelWidth = DECIMAL(2.0) / maskSize_;
    decimal offset     = DECIMAL(1.0) - DECIMAL(1.0) / maskSize_;

    constexpr int samplesPerPixel = 1028;
    decimal subStep   = pixelWidth / samplesPerPixel;
    decimal subOffset = subStep / 2;

    for (int i = 0; i < maskSize_; i++) {
        for (int j = 0; j < maskSize_; j++) {
            // Center of the pixel in the normalized window
            decimal uCenter = ((DECIMAL(2.0) * j) / maskSize_) - offset;
            decimal vCenter = ((DECIMAL(2.0) * i) / maskSize_) - offset;

            // Utilizing midpoint rule to approximate the integral in Eq.(72) with a Riemann sum
            decimal sum11 = 0;
            decimal sum20 = 0;

            for (int si = 0; si < samplesPerPixel; si++) {
                for (int sj = 0; sj < samplesPerPixel; sj++) {
                    decimal u = uCenter - pixelWidth / 2 + DECIMAL(sj)*subStep + subOffset;
                    decimal v = vCenter - pixelWidth / 2 + DECIMAL(si)*subStep + subOffset;

                    decimal rSquared = u*u + v*v;

                    if (rSquared <= 1) {  // check whether this part of pixel is inside the unit disk
                        sum11 += u;  // Eq.(50)
                        sum20 += DECIMAL(2.0)*rSquared - 1;  // Eq.(51)
                    }
                }
            }

            decimal areaWeight = (pixelWidth * pixelWidth) / (samplesPerPixel * samplesPerPixel);
            int idx = i * maskSize_ + j;

            kernelM11[idx].real = areaWeight * sum11;
            kernelM11[idx].imag = 0;

            kernelM20[idx].real = areaWeight * sum20;
            kernelM20[idx].imag = 0;
        }
    }

    // transpose of real component of M11 kernel is the imaginary component of M11 kernel (Eq.(75))
    for (int i = 0; i < maskSize_; i++) {
        for (int j = 0; j < maskSize_; j++) {
            kernelM11[i * maskSize_ + j].imag = kernelM11[j * maskSize_ + i].real;
        }
    }

    return {std::move(kernelM11), std::move(kernelM20)};
}

std::pair<ComplexNumber, ComplexNumber> ZernikeEdgeDetectionAlgorithm::computeZernikeMoments(
        const Image &image, const Vec2<int> &center, const std::unique_ptr<ComplexNumber[]> &kernelM11,
        const std::unique_ptr<ComplexNumber[]> &kernelM20) {
    decimal A11Real = 0;
    decimal A11Imag = 0;
    decimal A20Val = 0;
    int halfMask = maskSize_ / 2;

    for (int i = 0; i < maskSize_; i++) {
        for (int j = 0; j < maskSize_; j++) {
            // Using index of the pixel in the image to access pixel intensity
            int idx = i * maskSize_ + j;
            int imgIndex = (center.y + (i - halfMask)) * image.width + (center.x + (j - halfMask));
            decimal intensity = DECIMAL(image.image[imgIndex * image.channels]);

            // Eq.(71)
            A11Real += intensity * kernelM11[idx].real;
            A11Imag += intensity * kernelM11[idx].imag;
            A20Val += intensity * kernelM20[idx].real;
        }
    }

    return {ComplexNumber{A11Real, A11Imag}, ComplexNumber{A20Val, 0}};  // A_20 is real-only
}

decimal ZernikeEdgeDetectionAlgorithm::extractEdgeAngle(const ComplexNumber &A11) {
    return DECIMAL_ATAN2(A11.imag, A11.real);  // Eq.(56)
}

decimal ZernikeEdgeDetectionAlgorithm::extractEdgeOffset(decimal A11Prime, decimal A20) {
    if (DECIMAL_ABS(A11Prime) < DECIMAL(1e-10)) {  // Avoid division by zero
        return 0;
    }

    decimal wSqu = this->transitionWidth_ * this->transitionWidth_;

    decimal discriminant = (((wSqu - DECIMAL(1.0)) * (wSqu - DECIMAL(1.0))) - (DECIMAL(2.0) * wSqu * (A20 / A11Prime)));
    if (discriminant < 0) {
        return 0;
    }

    decimal l = (DECIMAL(1.0) - wSqu - DECIMAL_SQRT(discriminant)) / wSqu;  // Eq.(66)
    if (l < -1) {
        return -1;
    }

    return l;
}

Vec2<int> ZernikeEdgeDetectionAlgorithm::applyEdgeCorrection(const Vec2<int>& maskCenter, decimal l, decimal psi) {
    decimal scale = DECIMAL(maskSize_) / DECIMAL(2.0);
    int offsetX = static_cast<int>(scale * l * DECIMAL_COS(psi));  // Eq.(70)
    int offsetY = static_cast<int>(scale * l * DECIMAL_SIN(psi));  // Eq.(70)

    return Vec2<int>{maskCenter.x + offsetX, maskCenter.y + offsetY};
}

Points ZernikeEdgeDetectionAlgorithm::Run(const Image &image) {
    if (maskSize_ <= 0 || maskSize_ % 2 == 0) {
        throw std::invalid_argument("Mask size must be a positive odd integer");
    }

    Points initialPoints = initialEdgeAlgorithm_.Run(image);

    if (initialPoints.empty()) {
        return initialPoints;
    }

    // Step 1: Compute Zernike kernels
    const std::pair<std::unique_ptr<ComplexNumber[]>, std::unique_ptr<ComplexNumber[]>> kernels =
        computeZernikeKernels();
    const std::unique_ptr<ComplexNumber[]> &kernelM11 = kernels.first;
    const std::unique_ptr<ComplexNumber[]> &kernelM20 = kernels.second;

    // Step 2: Process each initial edge point and refine it using Zernike moments
    Points refinedPoints;

    for (const Vec2<> &initialPoint : initialPoints) {
        Vec2<int> center{
            static_cast<int>(std::lround(initialPoint.x)),
            static_cast<int>(std::lround(initialPoint.y))};

        // Step 2a: Check if the mask is within the image and if not, keep the initial point
        int halfMask = maskSize_ / 2;
        if (center.x < halfMask || center.y < halfMask || center.x + halfMask >= image.width
            || center.y + halfMask >= image.height) {
            refinedPoints.push_back(initialPoint);
            continue;
        }

        // Step 2b: Compute Zernike moments for the mask around center
        const std::pair<ComplexNumber, ComplexNumber> moments =
                computeZernikeMoments(image, center, kernelM11, kernelM20);
        const ComplexNumber &A11 = moments.first;

        // Step 2c: Extract edge angle ψ from A_11
        const decimal psi = extractEdgeAngle(A11);

        // Step 2d: Rotate A_11 to align with edge direction
        decimal A11Prime = A11.real * DECIMAL_COS(psi) + A11.imag * DECIMAL_SIN(psi);

        // Step 2e: Solve for edge distance l
        decimal l = extractEdgeOffset(A11Prime, moments.second.real);

        // Step 2f: Convert polar coordinates back to pixel coordinates
        Vec2<int> refinedPoint = applyEdgeCorrection(center, l, psi);
        refinedPoints.push_back({DECIMAL(refinedPoint.x), DECIMAL(refinedPoint.y)});
    }

    return refinedPoints;
}

}  // namespace found
