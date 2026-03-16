#include "distance/edge.hpp"

#include <unordered_map>
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#include <functional>
#include <unordered_set>
#include <cmath>

#include <Eigen/Core>
#include <unsupported/Eigen/CXX11/Tensor>

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

////// Kernel Edge Detection Algorithm //////

Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic>
    KernelEdgeDetectionAlgorithm::Convolve3x3(
        const Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> &gray,
        const Eigen::Tensor<decimal, 2> &kernel) const {
    const Eigen::Index rows = gray.rows();
    const Eigen::Index cols = gray.cols();
    Eigen::TensorMap<const Eigen::Tensor<decimal, 2>> image_tensor(gray.data(),
                                                                   rows, cols);
    Eigen::Tensor<decimal, 2> kernel_3x3(3, 3);
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            kernel_3x3(r, c) = kernel(r * 3 + c, 0);
    Eigen::array<Eigen::Index, 2> dims = {{0, 1}};
    Eigen::Tensor<decimal, 2> result =
        image_tensor.convolve(kernel_3x3, dims).eval();
    Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> out(rows - 2,
                                                                cols - 2);
    for (Eigen::Index r = 0; r < result.dimension(0); r++)
        for (Eigen::Index c = 0; c < result.dimension(1); c++)
            out(r, c) = result(r, c);
    return out;
}

////// Sobel Edge Detection Algorithm //////

SobelEdgeDetectionAlgorithm::SobelEdgeDetectionAlgorithm(decimal highThreshold)
    : highThreshold_(highThreshold) {
    gxKernel_.resize(9, 1);
    gyKernel_.resize(9, 1);
    const decimal gx_vals[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    const decimal gy_vals[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    for (int i = 0; i < 9; i++) {
        gxKernel_(i, 0) = DECIMAL(gx_vals[i]);
        gyKernel_(i, 0) = DECIMAL(gy_vals[i]);
    }
}

Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic>
    SobelEdgeDetectionAlgorithm::ToGrayscaleMatrix(const Image &image) const {
    const int rows = image.height;
    const int cols = image.width;
    Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> gray(rows, cols);
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            uint64_t idx = static_cast<uint64_t>(r * cols + c) * image.channels;
            decimal sum = 0;
            for (int ch = 0; ch < image.channels; ch++)
                sum += static_cast<decimal>(image.image[idx + ch]);
            gray(r, c) = sum / (image.channels * 255);
        }
    }
    return gray;
}

void SobelEdgeDetectionAlgorithm::ComputeMagnitudeAndDirection(
    const Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> &gx,
    const Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> &gy,
    Eigen::Array<decimal, Eigen::Dynamic, Eigen::Dynamic> *mag,
    Eigen::Array<decimal, Eigen::Dynamic, Eigen::Dynamic> *ratio) const {
    *mag = (gx.array().square() + gy.array().square()).sqrt();
    Eigen::Array<decimal, Eigen::Dynamic, Eigen::Dynamic> abs_gx =
        gx.array().abs();
    Eigen::Array<decimal, Eigen::Dynamic, Eigen::Dynamic> abs_gy =
        gy.array().abs();
    const decimal eps = DECIMAL(1e-5);
    *ratio = abs_gy / (abs_gx + eps);
}

Eigen::Array<bool, Eigen::Dynamic, Eigen::Dynamic>
    SobelEdgeDetectionAlgorithm::NonMaxSuppression(
        const Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> &gx,
        const Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> &gy,
        const Eigen::Array<decimal, Eigen::Dynamic, Eigen::Dynamic> &mag,
        const Eigen::Array<decimal, Eigen::Dynamic, Eigen::Dynamic> &ratio)
    const {
    const int nms_r = mag.rows() - 2;
    const int nms_c = mag.cols() - 2;
    auto mask_h = (ratio.block(1, 1, nms_r, nms_c) < DECIMAL(0.414)).eval();
    auto mask_v = (ratio.block(1, 1, nms_r, nms_c) > DECIMAL(2.414)).eval();
    auto sign_match = (gx.block(1, 1, nms_r, nms_c).array() *
                       gy.block(1, 1, nms_r, nms_c).array()) > 0;
    auto ratio_mid = (ratio.block(1, 1, nms_r, nms_c) >= DECIMAL(0.414) &&
                      ratio.block(1, 1, nms_r, nms_c) <= DECIMAL(2.414));
    auto mask_d45 = (ratio_mid && sign_match).eval();
    auto mask_d135 = (ratio_mid && !sign_match).eval();

    Eigen::Array<decimal, Eigen::Dynamic, Eigen::Dynamic> mag_center =
        mag.block(1, 1, nms_r, nms_c);
    return (mask_h && (mag_center > mag.block(1, 0, nms_r, nms_c)) &&
                    (mag_center > mag.block(1, 2, nms_r, nms_c))) ||
           (mask_v && (mag_center > mag.block(0, 1, nms_r, nms_c)) &&
                    (mag_center > mag.block(2, 1, nms_r, nms_c))) ||
           (mask_d45 && (mag_center > mag.block(0, 0, nms_r, nms_c)) &&
                    (mag_center > mag.block(2, 2, nms_r, nms_c))) ||
           (mask_d135 && (mag_center > mag.block(0, 2, nms_r, nms_c)) &&
                    (mag_center > mag.block(2, 0, nms_r, nms_c)));
}

Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic>
    SobelEdgeDetectionAlgorithm::HysteresisThreshold(
        const Eigen::Array<bool, Eigen::Dynamic, Eigen::Dynamic> &is_max,
        const Eigen::Array<decimal, Eigen::Dynamic, Eigen::Dynamic> &mag)
    const {
    const int inner_r = mag.rows();
    const int inner_c = mag.cols();
    const int nms_r = inner_r - 2;
    const int nms_c = inner_c - 2;
    Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> nms_result =
        Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic>::Zero(inner_r,
                                                                      inner_c);
    Eigen::Array<decimal, Eigen::Dynamic, Eigen::Dynamic> mag_center =
        mag.block(1, 1, nms_r, nms_c);
    for (int i = 0; i < nms_r; i++) {
        for (int j = 0; j < nms_c; j++) {
            if (is_max(i, j) && mag_center(i, j) > highThreshold_)
                nms_result(i + 1, j + 1) = mag_center(i, j);
        }
    }
    return nms_result;
}

Points SobelEdgeDetectionAlgorithm::CollectPointsAndSortPolar(
    const Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> &nms_result)
    const {
    const int inner_r = static_cast<int>(nms_result.rows());
    const int inner_c = static_cast<int>(nms_result.cols());
    Points result;
    for (int r = 0; r < inner_r; r++) {
        for (int c = 0; c < inner_c; c++) {
            if (nms_result(r, c) > highThreshold_)
                result.push_back(Vec2(DECIMAL(c + 1), DECIMAL(r + 1)));
        }
    }
    if (result.size() < 2) return result;
    Vec2 centroid(0, 0);
    for (const Vec2 &p : result) centroid += p;
    centroid /= static_cast<decimal>(result.size());
    std::sort(result.begin(), result.end(),
              [&centroid](const Vec2 &a, const Vec2 &b) {
                  return DECIMAL_ATAN2(a.y() - centroid.y(), a.x() - centroid.x()) <
                         DECIMAL_ATAN2(b.y() - centroid.y(), b.x() - centroid.x());
              });
    return result;
}

Points SobelEdgeDetectionAlgorithm::Run(const Image &image) {
    const int rows = image.height;
    const int cols = image.width;
    if (rows < 3 || cols < 3) return Points();

    Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> gray =
        ToGrayscaleMatrix(image);
    Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> gx =
        Convolve3x3(gray, gxKernel_);
    Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> gy =
        Convolve3x3(gray, gyKernel_);

    Eigen::Array<decimal, Eigen::Dynamic, Eigen::Dynamic> mag, ratio;
    ComputeMagnitudeAndDirection(gx, gy, &mag, &ratio);

    const int inner_r = rows - 2;
    const int inner_c = cols - 2;
    if (inner_r < 3 || inner_c < 3) return Points();

    Eigen::Array<bool, Eigen::Dynamic, Eigen::Dynamic> is_max =
        NonMaxSuppression(gx, gy, mag, ratio);
    Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> nms_result =
        HysteresisThreshold(is_max, mag);

    return CollectPointsAndSortPolar(nms_result);
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

////// Zernike Edge Detection Algorithm //////

std::unique_ptr<decimal[]> ZernikeEdgeDetectionAlgorithm::extractWindow(
    const Image &image, const Vec2 &center) {
    std::unique_ptr<decimal[]> window(new decimal[windowSize * windowSize]);
    int halfWindow = windowSize / 2;

    // Window is entered around edge pixel
    int centerXInt = static_cast<int>(DECIMAL_ROUND(center.x()));
    int centerYInt = static_cast<int>(DECIMAL_ROUND(center.y()));

    for (int i = 0; i < windowSize; i++) {
        for (int j = 0; j < windowSize; j++) {
            int imgX = centerXInt + (j - halfWindow);
            int imgY = centerYInt + (i - halfWindow);

            // Handle boundaries: use edge values
            imgX = std::max(0, std::min(imgX, image.width - 1));
            imgY = std::max(0, std::min(imgY, image.height - 1));

            uint64_t imgIndex = static_cast<uint64_t>(imgY * image.width + imgX);
            window[i * windowSize + j] = static_cast<decimal>(image.image[imgIndex * image.channels]);
        }
    }

    return window;
}

Eigen::Matrix<decimal, Eigen::Dynamic, 3>
    ZernikeEdgeDetectionAlgorithm::computeZernikeKernels() {
    const int N = windowSize * windowSize;
    Eigen::Matrix<decimal, Eigen::Dynamic, 3> K(N, 3);

    decimal pixelWidth = DECIMAL(2.0) / DECIMAL(windowSize);
    decimal offset     = DECIMAL(1.0) - DECIMAL(1.0) / DECIMAL(windowSize);

    constexpr int SUB = 16;
    decimal subStep   = pixelWidth / DECIMAL(SUB);
    decimal subOffset = subStep / DECIMAL(2.0);

    for (int i = 0; i < windowSize; i++) {
        for (int j = 0; j < windowSize; j++) {
            decimal uCenter = DECIMAL(2.0) * DECIMAL(j) / DECIMAL(windowSize) - offset;
            decimal vCenter = DECIMAL(2.0) * DECIMAL(i) / DECIMAL(windowSize) - offset;

            decimal sum11 = DECIMAL(0.0);
            decimal sum20 = DECIMAL(0.0);

            for (int si = 0; si < SUB; si++) {
                for (int sj = 0; sj < SUB; sj++) {
                    decimal u = uCenter - pixelWidth / DECIMAL(2.0) +
                               DECIMAL(sj) * subStep + subOffset;
                    decimal v = vCenter - pixelWidth / DECIMAL(2.0) +
                               DECIMAL(si) * subStep + subOffset;

                    decimal rSquared = u * u + v * v;

                    if (rSquared <= DECIMAL(1.0)) {
                        sum11 += u;
                        sum20 += DECIMAL(2.0) * rSquared - DECIMAL(1.0);
                    }
                }
            }

            decimal areaWeight = pixelWidth * pixelWidth / DECIMAL(SUB * SUB);

            int idx = i * windowSize + j;
            K(idx, 0) = areaWeight * sum11;   // Z_11 real
            K(idx, 2) = areaWeight * sum20;  // Z_20 real
        }
    }

    // Z_11 imag = transpose of Z_11 real
    for (int i = 0; i < windowSize; i++) {
        for (int j = 0; j < windowSize; j++) {
            K(i * windowSize + j, 1) = K(j * windowSize + i, 0);
        }
    }

    return K;
}

std::pair<ComplexNumber, ComplexNumber> ZernikeEdgeDetectionAlgorithm::computeZernikeMoments(
    const decimal* window,
    const Eigen::Matrix<decimal, Eigen::Dynamic, 3> &kernelMatrix) {
    const int N = windowSize * windowSize;
    Eigen::Map<const Eigen::Matrix<decimal, Eigen::Dynamic, 1>> w(window, N, 1);
    Eigen::Matrix<decimal, 3, 1> r = kernelMatrix.transpose() * w;

    ComplexNumber A11 = {r(0), r(1)};
    ComplexNumber A20 = {r(2), DECIMAL(0.0)};  // A_20 is real-only
    return {A11, A20};
}

decimal ZernikeEdgeDetectionAlgorithm::extractEdgeAngle(const ComplexNumber& A11) {
    return DECIMAL_ATAN2(A11.imag, A11.real);
}

decimal ZernikeEdgeDetectionAlgorithm::solveEdgeDistance(decimal A11Prime, decimal A20, decimal transitionWidth) {
    if (DECIMAL_ABS(A11Prime) < DECIMAL(1e-10)) {
        return DECIMAL(0.0);
    }
    decimal w = transitionWidth;
    decimal wSqu = w * w;

    decimal discriminant = ((wSqu - DECIMAL(1.0)) * (wSqu - DECIMAL(1.0)) - DECIMAL(2.0) * wSqu * (A20 / A11Prime));
    return (DECIMAL(1.0) - wSqu - sqrt(discriminant)) / wSqu;
}

Vec2 ZernikeEdgeDetectionAlgorithm::convertPolarToPixel(const Vec2& windowCenter, decimal l, decimal psi) {
    decimal scale = DECIMAL(windowSize) / DECIMAL(2.0);
    decimal offsetX = scale * l * DECIMAL_COS(psi);
    decimal offsetY = scale * l * DECIMAL_SIN(psi);

    return Vec2{windowCenter.x() + offsetX, windowCenter.y() + offsetY};
}

Points ZernikeEdgeDetectionAlgorithm::Run(const Image &image) {
    if (windowSize <= 0 || windowSize % 2 == 0) {
        throw std::invalid_argument("windowSize must be a positive odd integer");
    }

    Points initialPoints = initialEdgeAlgorithm->Run(image);

    if (initialPoints.empty()) {
        return initialPoints;
    }

    // Step 1: Compute Zernike kernels (single matrix for efficient K^T * w)
    Eigen::Matrix<decimal, Eigen::Dynamic, 3> kernelMatrix = computeZernikeKernels();

    // Step 2: Process each initial edge point
    Points refinedPoints;

    for (const Vec2& initialPoint : initialPoints) {
        // Step 2a: Extract window around the point
        std::unique_ptr<decimal[]> window = extractWindow(image, initialPoint);

        // Step 2b: Compute Zernike moments via one matrix-vector multiply
        ComplexNumber A11;
        ComplexNumber A20;
        std::tie(A11, A20) = computeZernikeMoments(window.get(), kernelMatrix);

        // Step 2c: Extract edge angle ψ from A_11
        decimal psi = extractEdgeAngle(A11);

        // Step 2d: Rotate A_11 to align with edge direction
        decimal cosPsi = DECIMAL_COS(psi);
        decimal sinPsi = DECIMAL_SIN(psi);
        decimal A11Prime = A11.real * cosPsi + A11.imag * sinPsi;

        // Step 2e: Solve for edge distance l using Christian's equations
        decimal l = solveEdgeDistance(A11Prime, A20.real, transitionWidth);
        if (!std::isfinite(static_cast<double>(l))) {
            refinedPoints.push_back(initialPoint);
            continue;
        }

        // Step 2f: Convert back to pixel coordinates
        Vec2 refinedPoint = convertPolarToPixel(initialPoint, l, psi);
        if (!std::isfinite(static_cast<double>(refinedPoint.x())) ||
            !std::isfinite(static_cast<double>(refinedPoint.y()))) {
            refinedPoints.push_back(initialPoint);
            continue;
        }

        refinedPoints.push_back(refinedPoint);
    }

    return refinedPoints;
}

}  // namespace found
