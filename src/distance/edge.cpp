#include "distance/edge.hpp"

#include <unordered_map>
#include <algorithm>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>
#include <functional>
#include <unordered_set>
#include <limits>
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

////// Zernike Edge Detection Algorithm //////

std::unique_ptr<decimal[]> ZernikeEdgeDetectionAlgorithm::imageToGrayscaleDouble(const Image &image) {
    uint64_t imageSize = static_cast<uint64_t>(image.width * image.height);
    std::unique_ptr<decimal[]> result(new decimal[imageSize]);

    for (uint64_t i = 0; i < imageSize; i++) {
        // Average channels to get grayscale
        int sum = 0;
        for (int c = 0; c < image.channels; c++) {
            sum += image.image[image.channels * i + c];
        }
        result[i] = DECIMAL(sum) / DECIMAL(image.channels);
    }

    return result;
}

std::unique_ptr<decimal[]> ZernikeEdgeDetectionAlgorithm::extractWindow(
    const decimal* imageData,
    int imageWidth,
    int imageHeight,
    decimal centerX,
    decimal centerY,
    int windowSize
) {
    std::unique_ptr<decimal[]> window(new decimal[windowSize * windowSize]);
    int halfWindow = windowSize / 2;

    // Window is entered around edge pixel
    int centerXInt = static_cast<int>(DECIMAL_ROUND(centerX));
    int centerYInt = static_cast<int>(DECIMAL_ROUND(centerY));

    for (int i = 0; i < windowSize; i++) {
        for (int j = 0; j < windowSize; j++) {
            int imgX = centerXInt + (j - halfWindow);
            int imgY = centerYInt + (i - halfWindow);

            // Handle boundaries: use edge values
            imgX = std::max(0, std::min(imgX, imageWidth - 1));
            imgY = std::max(0, std::min(imgY, imageHeight - 1));

            uint64_t imgIndex = static_cast<uint64_t>(imgY * imageWidth + imgX);
            window[i * windowSize + j] = imageData[imgIndex];
        }
    }

    return window;
}

std::tuple<std::unique_ptr<decimal[]>, std::unique_ptr<decimal[]>, std::unique_ptr<decimal[]>>
ZernikeEdgeDetectionAlgorithm::computeZernikeKernels(int windowSize) {
    int n2 = windowSize * windowSize;
    std::unique_ptr<decimal[]> kernelZ11Real(new decimal[n2]);
    std::unique_ptr<decimal[]> kernelZ11Imag(new decimal[n2]);
    std::unique_ptr<decimal[]> kernelZ20(new decimal[n2]);

    decimal* kernelZ11RealPtr = kernelZ11Real.get();
    decimal* kernelZ11ImagPtr = kernelZ11Imag.get();
    decimal* kernelZ20Ptr     = kernelZ20.get();

    // Pixel width in normalized coordinates = 2/N
    decimal pixelWidth = DECIMAL(2.0) / DECIMAL(windowSize);
    decimal offset     = DECIMAL(1.0) - DECIMAL(1.0) / DECIMAL(windowSize);

    // Number of sub-samples per pixel side for the area integral
    // 16x16 = 256 samples per pixel
    constexpr int SUB = 16;
    decimal subStep   = pixelWidth / DECIMAL(SUB);
    decimal subOffset = subStep / DECIMAL(2.0);  // center of first sub-cell

    for (int i = 0; i < windowSize; i++) {
        for (int j = 0; j < windowSize; j++) {
            // Centre of pixel (i,j) in normalized coords
            decimal uCenter = DECIMAL(2.0) * DECIMAL(j) / DECIMAL(windowSize) - offset;
            decimal vCenter = DECIMAL(2.0) * DECIMAL(i) / DECIMAL(windowSize) - offset;

            decimal sum11 = DECIMAL(0.0);
            decimal sum20 = DECIMAL(0.0);

            for (int si = 0; si < SUB; si++) {
                for (int sj = 0; sj < SUB; sj++) {
                    decimal u = uCenter - pixelWidth / DECIMAL(2.0)
                                + DECIMAL(sj) * subStep + subOffset;
                    decimal v = vCenter - pixelWidth / DECIMAL(2.0)
                                + DECIMAL(si) * subStep + subOffset;

                    decimal rSquared = u * u + v * v;
                    if (rSquared <= DECIMAL(1.0)) {
                        sum11 += u;                               // Re[T_11] = u
                        sum20 += DECIMAL(2.0) * rSquared - DECIMAL(1.0);  // T_20
                    }
                }
            }

            // Multiply by pixel area / num_subsamples = pixelWidth² / SUB²
            decimal areaWeight = pixelWidth * pixelWidth / DECIMAL(SUB * SUB);
            int idx = i * windowSize + j;
            kernelZ11RealPtr[idx] = areaWeight * sum11;
            kernelZ20Ptr[idx]     = areaWeight * sum20;
        }
    }

    // Imaginary kernel of Z_11 is the transpose of real
    for (int i = 0; i < windowSize; i++) {
        for (int j = 0; j < windowSize; j++) {
            kernelZ11ImagPtr[i * windowSize + j] = kernelZ11RealPtr[j * windowSize + i];
        }
    }

    return std::make_tuple(
        std::move(kernelZ11Real),
        std::move(kernelZ11Imag),
        std::move(kernelZ20));
}

std::pair<ZernikeMoment, ZernikeMoment> ZernikeEdgeDetectionAlgorithm::computeZernikeMoments(
    const decimal* window,
    const decimal* kernelZ11Real,
    const decimal* kernelZ11Imag,
    const decimal* kernelZ20,
    int windowSize
) {
    decimal A11Real = DECIMAL(0.0);
    decimal A11Imag = DECIMAL(0.0);
    decimal A20Val = DECIMAL(0.0);

    for (int i = 0; i < windowSize; i++) {
        for (int j = 0; j < windowSize; j++) {
            int idx = i * windowSize + j;
            decimal intensity = window[idx];

            A11Real += intensity * kernelZ11Real[idx];
            A11Imag += intensity * kernelZ11Imag[idx];
            A20Val += intensity * kernelZ20[idx];
        }
    }

    ZernikeMoment A11 = {A11Real, A11Imag};
    ZernikeMoment A20 = {A20Val, DECIMAL(0.0)};  // A_20 is real-only
    return {A11, A20};
}

decimal ZernikeEdgeDetectionAlgorithm::extractEdgeAngle(const ZernikeMoment& A11) {
    return DECIMAL_ATAN2(A11.imag, A11.real);
}

decimal ZernikeEdgeDetectionAlgorithm::solveEdgeDistance(
    decimal A11Prime,
    decimal A20,
    decimal transitionWidth) {
    if (DECIMAL_ABS(A11Prime) < DECIMAL(1e-10)) {
        return DECIMAL(0.0);
    }
    decimal w = transitionWidth;
    decimal wSqu = w * w;

    decimal discriminant = ((wSqu - DECIMAL(1.0)) * (wSqu - DECIMAL(1.0)) - DECIMAL(2.0) * wSqu * (A20 / A11Prime));
    return (DECIMAL(1.0) - wSqu - sqrt(discriminant)) / wSqu;
}

Vec2 ZernikeEdgeDetectionAlgorithm::convertPolarToPixel(
    const Vec2& windowCenter,
    decimal l,
    decimal psi,
    int windowSize
) {
    decimal scale = DECIMAL(windowSize) / DECIMAL(2.0);
    decimal offsetX = scale * l * DECIMAL_COS(psi);
    decimal offsetY = scale * l * DECIMAL_SIN(psi);

    return Vec2{windowCenter.x + offsetX, windowCenter.y + offsetY};
}

Points ZernikeEdgeDetectionAlgorithm::Run(const Image &image) {
    return Run(image, initialEdgeAlgorithm_->Run(image));
}

Points ZernikeEdgeDetectionAlgorithm::Run(const Image &image, const Points &initialPoints) {
    if (initialPoints.empty()) {
        return initialPoints;
    }

    // Step 1: Convert image to grayscale double precision
    std::unique_ptr<decimal[]> imageData = imageToGrayscaleDouble(image);

    // Step 2: Compute Zernike kernels once (reused for all windows)
    std::unique_ptr<decimal[]> kernelZ11Real;
    std::unique_ptr<decimal[]> kernelZ11Imag;
    std::unique_ptr<decimal[]> kernelZ20;
    std::tie(kernelZ11Real, kernelZ11Imag, kernelZ20) = computeZernikeKernels(windowSize_);

    // Step 3: Process each initial edge point
    Points refinedPoints;

    for (const Vec2& initialPoint : initialPoints) {
        // Step 3a: Extract window around the point
        std::unique_ptr<decimal[]> window = extractWindow(
            imageData.get(),
            image.width,
            image.height,
            initialPoint.x,
            initialPoint.y,
            windowSize_);

        // Step 3b: Compute Zernike moments for this window
        ZernikeMoment A11;
        ZernikeMoment A20;
        std::tie(A11, A20) = computeZernikeMoments(
            window.get(),
            kernelZ11Real.get(),
            kernelZ11Imag.get(),
            kernelZ20.get(),
            windowSize_);

        // Step 3c: Extract edge angle ψ from A_11
        decimal psi = extractEdgeAngle(A11);

        // Step 3d: Rotate A_11 to align with edge direction
        decimal cosPsi = DECIMAL_COS(psi);
        decimal sinPsi = DECIMAL_SIN(psi);
        decimal A11Prime = A11.real * cosPsi + A11.imag * sinPsi;

        // Step 3e: Solve for edge distance l using Christian's equations
        decimal l = solveEdgeDistance(A11Prime, A20.real, transitionWidth_);
        if (!std::isfinite(static_cast<double>(l))) {
            refinedPoints.push_back(initialPoint);
            continue;
        }

        // Step 3f: Convert back to pixel coordinates
        Vec2 refinedPoint = convertPolarToPixel(initialPoint, l, psi, windowSize_);
        if (!std::isfinite(static_cast<double>(refinedPoint.x)) ||
            !std::isfinite(static_cast<double>(refinedPoint.y))) {
            refinedPoints.push_back(initialPoint);
            continue;
        }

        refinedPoints.push_back(refinedPoint);
    }

    return refinedPoints;
}

}  // namespace found
