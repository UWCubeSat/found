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

InertialSymmetryEdgeDetectionAlgorithm::InertialSymmetryEdgeDetectionAlgorithm(
    uint8_t grayThreshold,
    int lineCount,
    decimal lineEpsilon,
    const Eigen::Matrix<decimal, Eigen::Dynamic, 1>& mask,
    decimal sparseness)
    : grayThreshold_(grayThreshold),
      lineCount_(lineCount),
      lineEpsilon_(lineEpsilon),
      sparseness_(sparseness) {
    if (mask.size() == 0) {
        mask_.resize(8);
        mask_ << 0, 0, 0, 0, 1, 1, 1, 1;
    } else {
        mask_ = mask;
    }
}

Vec2 InertialSymmetryEdgeDetectionAlgorithm::findImageEdge(
    int imageWidth, int imageHeight,
    const Vec2& start, const Vec2& direction) const {
    imageWidth -= 1;
    imageHeight -= 1;

    if (direction.x() == 0 && direction.y() == 0) {
        return start;
    }

    if (direction.x() == 0) {
        return direction.y() < 0 ? Vec2(start.x(), 0) : Vec2(start.x(), imageHeight);
    }
    if (direction.y() == 0) {
        return direction.x() < 0 ? Vec2(0, start.y()) : Vec2(imageWidth, start.y());
    }

    decimal slope = direction.y() / direction.x();

    decimal x1 = start.x() - start.y() / slope;
    if (x1 >= 0 && x1 <= imageWidth && direction.y() < 0) {
        return Vec2(x1, 0);
    }

    decimal x2 = start.x() - (start.y() - imageHeight) / slope;
    if (x2 >= 0 && x2 <= imageWidth && direction.y() > 0) {
        return Vec2(x2, imageHeight);
    }

    decimal y1 = start.y() - start.x() * slope;
    if (y1 >= 0 && y1 <= imageHeight && direction.x() < 0) {
        return Vec2(0, y1);
    }

    decimal y2 = start.y() - (start.x() - imageWidth) * slope;
    return Vec2(imageWidth, y2);
}

decimal InertialSymmetryEdgeDetectionAlgorithm::sampleImageBilinear(
    int imageWidth, int imageHeight,
    const Eigen::Matrix<decimal, Eigen::Dynamic, 1>& pixels,
    const Vec2& position) const {
    int ix = static_cast<int>(position.x());
    int iy = static_cast<int>(position.y());

    if (ix + 1 >= imageWidth || iy + 1 >= imageHeight) {
        return DECIMAL(0.0);
    }

    decimal px = position.x() - ix;
    decimal py = position.y() - iy;

    decimal v0 = pixels(iy * imageWidth + ix);
    decimal v1 = pixels(iy * imageWidth + (ix + 1));
    decimal v2 = pixels((iy + 1) * imageWidth + ix);
    decimal v3 = pixels((iy + 1) * imageWidth + (ix + 1));

    return (DECIMAL(1.0) - py) *
        (v0 * (DECIMAL(1.0) - px) + v1 * px) +
        py * (v2 * (DECIMAL(1.0) - px) + v3 * px);
}

decimal InertialSymmetryEdgeDetectionAlgorithm::ellipseFit(
    const Eigen::Matrix<decimal, Eigen::Dynamic, 2>& points) const {
    if (points.rows() == 0) {
        return DECIMAL(0.0);
    }
    Eigen::Matrix<decimal, Eigen::Dynamic, 6> ellipseEquations(points.rows(), 6);
    for (Eigen::Index i = 0; i < points.rows(); ++i) {
        decimal x = points(i, 0);
        decimal y = points(i, 1);
        ellipseEquations.row(i) << x * x, x * y, y * y, x, y, 1;
    }

    Eigen::JacobiSVD<Eigen::Matrix<decimal, Eigen::Dynamic, 6>> svd(
        ellipseEquations, Eigen::ComputeFullV);
    Eigen::Matrix<decimal, 6, 6> VT = svd.matrixV();
    Eigen::Matrix<decimal, 6, 1> finalCol = VT.col(VT.cols() - 1);
    return svd.singularValues().dot(finalCol);
}

Points InertialSymmetryEdgeDetectionAlgorithm::matrixToPoints(
    const Eigen::Matrix<decimal, Eigen::Dynamic, 2>& M) const {
    Points out;
    out.reserve(static_cast<size_t>(M.rows()));
    for (Eigen::Index i = 0; i < M.rows(); ++i) {
        out.push_back(Vec2(M(i, 0), M(i, 1)));
    }
    return out;
}



Points InertialSymmetryEdgeDetectionAlgorithm::Run(const Image &image) {
    // Assumes image has a single channel (grayscale); index as image.image[i].
    const int npixels = image.width * image.height;
    Eigen::Matrix<decimal, Eigen::Dynamic, 1> binary_image(npixels);
    const Eigen::Index bodyCapacity = static_cast<Eigen::Index>(std::max(
        DECIMAL(1.0), std::min(DECIMAL(npixels), npixels * sparseness_)));
    Eigen::Matrix<decimal, Eigen::Dynamic, 2> body_index(bodyCapacity, 2);
    int rowCount = 0;

    for (int y = 0; y < image.height; ++y) {
        for (int x = 0; x < image.width; ++x) {
            decimal value = DECIMAL(image.image[y * image.width + x]);
            const bool on = (value > grayThreshold_);
            binary_image(y * image.width + x) = on ? DECIMAL(1.0) : DECIMAL(0.0);
            if (on) {
                body_index(rowCount, 0) = DECIMAL(x);
                body_index(rowCount, 1) = DECIMAL(y);
                rowCount++;
            }
        }
    }

    if (rowCount == 0) {
        return Points();
    }
    body_index.resize(rowCount, 2);

    // Step 2: Centroid
    Eigen::Matrix<decimal, 1, 2> centroidRow = body_index.colwise().mean();

    // Step 4: Centralize (shift coordinates to origin)
    body_index.rowwise() -= centroidRow;

    // Step 5: Inertia tensor (scatter matrix) body_index^T * body_index
    Mat2 tensor = body_index.transpose() * body_index;

    Eigen::JacobiSVD<Mat2> svd(tensor, Eigen::ComputeFullU);
    Vec2 wmax = svd.matrixU().col(0);
    Vec2 wmin = svd.matrixU().col(1);
    int mass = rowCount;
    decimal estimatedRadius = 2 * DECIMAL_SQRT(svd.singularValues()(0) / mass);
    Vec2 centroid = centroidRow.transpose();

    // Offset between scan lines. We use lineCount_/2 as the half-extent so the
    // number of lines is approximately lineCount_. The count is always odd
    // because of the center line at i = 0.
    const int halfCount = std::max(1, lineCount_ / 2);
    Vec2 offset = wmin * estimatedRadius / halfCount;

    const Eigen::Index maskSize = mask_.size();
    const int numLines = 2 * halfCount + 1;
    Eigen::Matrix<decimal, Eigen::Dynamic, 2> redMat(numLines, 2);
    Eigen::Matrix<decimal, Eigen::Dynamic, 2> blueMat(numLines, 2);
    int lineRows = 0;

    for (int i = -halfCount; i <= halfCount; i++) {
        Vec2 start = findImageEdge(
            image.width, image.height,
            centroid + (offset * i),
            wmax);
        Vec2 end = findImageEdge(
            image.width, image.height,
            centroid + (offset * i),
            -wmax);

        decimal length = (end - start).norm();
        if (length < lineEpsilon_) {
            continue;
        }

        Vec2 dir = (end - start) / length;

        int maxLength = maskSize <= length ? static_cast<int>(length) : static_cast<int>(maskSize);
        int minLength = maskSize > length ? static_cast<int>(length) : static_cast<int>(maskSize);
        int validLength = maxLength - minLength + 1;

        Eigen::Matrix<decimal, Eigen::Dynamic, 1> correlation(validLength);
        correlation.setZero();
        decimal maxCorrelation = 0;
        for (int n = 0; n < length; n++) {
            Vec2 point = start + dir * n;
            decimal sample = sampleImageBilinear(image.width, image.height, binary_image, point);
            for (int k = 0; k < validLength; k++) {
                int index = n - k;
                if (index >= 0 && index < maskSize) {
                    correlation(k) += mask_(index) * sample;
                    if (correlation(k) > maxCorrelation) {
                        maxCorrelation = correlation(k);
                    }
                }
            }
        }

        int i0 = 0;
        for (; i0 < validLength; i0++) {
            if (correlation(i0) >= maxCorrelation / 2) {
                break;
            }
        }
        int i1 = validLength - 1;
        for (; i1 >= 0; i1--) {
            if (correlation(i1) >= maxCorrelation / 2) {
                break;
            }
        }

        Vec2 p0 = start + dir * i0;
        Vec2 p1 = start + dir * i1;

        redMat.row(lineRows) << p0.x(), p0.y();
        blueMat.row(lineRows) << p1.x(), p1.y();
        lineRows++;
    }

    if (lineRows == 0) {
        return Points();
    }

    redMat.conservativeResize(lineRows, 2);
    blueMat.conservativeResize(lineRows, 2);

    decimal redError = ellipseFit(redMat);
    decimal blueError = ellipseFit(blueMat);

    if (redError < blueError) {
        return matrixToPoints(redMat);
    }
    return matrixToPoints(blueMat);
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
            window[i * windowSize + j] = DECIMAL(image.image[imgIndex * image.channels]);
        }
    }

    return window;
}

std::pair<std::unique_ptr<ComplexNumber[]>, std::unique_ptr<ComplexNumber[]>>
        ZernikeEdgeDetectionAlgorithm::computeZernikeKernels() {
    std::unique_ptr<ComplexNumber[]> kernelZ11(new ComplexNumber[windowSize * windowSize]);
    std::unique_ptr<ComplexNumber[]> kernelZ20(new ComplexNumber[windowSize * windowSize]);

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
                    decimal u = uCenter - pixelWidth/DECIMAL(2.0)
                                + DECIMAL(sj)*subStep + subOffset;
                    decimal v = vCenter - pixelWidth/DECIMAL(2.0)
                                + DECIMAL(si)*subStep + subOffset;

                    decimal rSquared = u*u + v*v;

                    if (rSquared <= DECIMAL(1.0)) {
                        sum11 += u;
                        sum20 += DECIMAL(2.0)*rSquared - DECIMAL(1.0);
                    }
                }
            }

            decimal areaWeight =
                pixelWidth * pixelWidth / DECIMAL(SUB * SUB);

            int idx = i * windowSize + j;

            kernelZ11[idx].real = areaWeight * sum11;
            kernelZ11[idx].imag = DECIMAL(0.0);  // filled later

            kernelZ20[idx].real = areaWeight * sum20;
            kernelZ20[idx].imag = DECIMAL(0.0);
        }
    }

    // transpose → imaginary component
    for (int i = 0; i < windowSize; i++) {
        for (int j = 0; j < windowSize; j++) {
            kernelZ11[i*windowSize + j].imag =
                kernelZ11[j*windowSize + i].real;
        }
    }

    return {std::move(kernelZ11), std::move(kernelZ20)};
}

std::pair<ComplexNumber, ComplexNumber> ZernikeEdgeDetectionAlgorithm::computeZernikeMoments(
    const decimal* window, const ComplexNumber* kernelZ11, const ComplexNumber* kernelZ20) {
    decimal A11Real = DECIMAL(0.0);
    decimal A11Imag = DECIMAL(0.0);
    decimal A20Val = DECIMAL(0.0);

    for (int i = 0; i < windowSize; i++) {
        for (int j = 0; j < windowSize; j++) {
            int idx = i * windowSize + j;
            decimal intensity = window[idx];

            A11Real += intensity * kernelZ11[idx].real;
            A11Imag += intensity * kernelZ11[idx].imag;
            A20Val += intensity * kernelZ20[idx].real;
        }
    }

    ComplexNumber A11 = {A11Real, A11Imag};
    ComplexNumber A20 = {A20Val, DECIMAL(0.0)};  // A_20 is real-only
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

    // Step 1: Compute Zernike kernels
    auto [kernelZ11, kernelZ20] = computeZernikeKernels();

    // Step 2: Process each initial edge point
    Points refinedPoints;

    for (const Vec2& initialPoint : initialPoints) {
        // Step 2a: Extract window around the point
        std::unique_ptr<decimal[]> window = extractWindow(image, initialPoint);

        // Step 2b: Compute Zernike moments for this window
        ComplexNumber A11;
        ComplexNumber A20;
        std::tie(A11, A20) = computeZernikeMoments(
            window.get(),
            kernelZ11.get(),
            kernelZ20.get());

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
