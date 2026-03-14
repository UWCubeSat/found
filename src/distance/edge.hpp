#ifndef SRC_DISTANCE_EDGE_HPP_
#define SRC_DISTANCE_EDGE_HPP_

#include <memory>
#include <functional>
#include <utility>

#include "common/style.hpp"
#include "common/decimal.hpp"
#include "common/pipeline/stages.hpp"

namespace found {

/**
 * The EdgeDetection Algorithm class houses the Edge Detection Algorithm. This algorithm uses 
 * a picture of Earth and finds all points on the horizon within the picture.
*/
class EdgeDetectionAlgorithm : public FunctionStage<Image, Points> {};

/**
 * The SimpleEdgeDetection Algorithm class houses the Edge Detection Algorithm. This algorithm uses 
 * a picture of Earth and finds all points on the horizon within the picture by employing thresholding
 * to identify "space", and then figure out the contour of "space" that is shared by Earth's edge, returning
 * that as the result
*/
class SimpleEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
 public:
    /**
     * @brief Constructs a new SimpleEdgeDetectionAlgorithm
     * 
     * @param threshold The threshold to use for detecting space
     * @param borderLength The thickness of the image's borders
     * @param offset The offset to apply to edge points
     */
    SimpleEdgeDetectionAlgorithm(unsigned char threshold, int borderLength, decimal offset) :
        threshold_(threshold), borderLength_(borderLength), offset_(offset) {}

    /// @brief Destroys the algorithm
    virtual ~SimpleEdgeDetectionAlgorithm() {}

    /**
     * Provides an estimate of the edge points of Earth, as
     * the shared edge between space and Earth.
     * 
     * @param image The image of Earth
     * 
     * @return The points on Earth's edge in image
     * 
     * @post The edge points returned are in polar order, i.e.
     * if we define the centroid of the points as P, for any
     * three consecutive points A B and C, angle APB is less than
     * angle APC
     */
    Points Run(const Image &image) override;

 private:
    /// The space-ether threshold to use
    unsigned char threshold_;
    /// The border length to use
    int borderLength_;
    /// The edge offset to use
    decimal offset_;
};

/**
 * The LoGEdgeDetection Algorithm class houses the Edge Detection Algorithm. This algorithm uses 
 * a picture of Earth and finds all points on the horizon within the picture by employing a 
 * Laplacian of Gaussian (LoG) filter to the image.
*/
class LoGEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
 public:
    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    LoGEdgeDetectionAlgorithm(/*Put more fields here!*/);

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    virtual ~LoGEdgeDetectionAlgorithm(/*Put more fields here!*/);

    /**
     * Place documentation here. Press enter to automatically make a new line
     * 
     * @param image The image to process
     * 
     * @return The edge points in the image most closely resembling that of earth
     * */
    Points Run(const Image &image) override;
 private:
    // useful fields specific to this algorithm and helper methods
};

/**
 * This class represents the Inertial Symmetry Edge Detection Algorithm, which
 * uses an approximation of the Earth's inertial parameters to guess the radius
 * before finding the points on the horizon.
 */
class InertialSymmetryEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
 public:
    /**
     * Instantiates a new instance of this algorithm using the given
     * grayThreshold value and a lineCount (equal to the maximum number of
     * points that can be emitted). A line must be greater than lineEpsilon
     * length.
     *
     * @param grayThreshold Threshold for binarizing the image
     * @param lineCount Maximum number of scan lines (and approximate point count).
     *        For accuracy, use an odd value greater than 3.
     * @param lineEpsilon Minimum line length to consider
     * @param mask Correlation mask for edge detection along each scan line.
     *        If empty (size 0), the default half-plane mask (0,0,0,0,1,1,1,1)
     *        is used.
     * @param sparseness Estimated fraction of pixels that are active (on), in
     *        [0, 1]. Used for allocation strategy; default 1 (assume dense).
     */
    InertialSymmetryEdgeDetectionAlgorithm(
        uint8_t grayThreshold,
        int lineCount,
        decimal lineEpsilon,
        const Eigen::Matrix<decimal, Eigen::Dynamic, 1>& mask,
        decimal sparseness = DECIMAL(1.0));

    /// @brief Destroys the algorithm
    virtual ~InertialSymmetryEdgeDetectionAlgorithm() {}

    /**
     * Provides an estimate of the edge points of Earth, as
     * the shared edge between space and Earth.
     *
     * @param image The image of Earth
     *
     * @return The points on Earth's edge in image
     */
    Points Run(const Image &image) override;

 private:
    /**
     * Finds the intersection of the ray from start in direction with the image
     * rectangle [0, imageWidth] x [0, imageHeight].
     *
     * @param imageWidth Image width in pixels
     * @param imageHeight Image height in pixels
     * @param start Ray start point
     * @param direction Ray direction (need not be normalized)
     * @return Intersection point on the image boundary
     */
    Vec2 findImageEdge(int imageWidth, int imageHeight,
                      const Vec2& start, const Vec2& direction) const;

    /**
     * Bilinear sample of the binary pixel array at the given position.
     *
     * @param imageWidth Image width
     * @param imageHeight Image height
     * @param pixels Binary thresholded image (Eigen vector, row-major index; values 0 or 1)
     * @param position Sub-pixel position (x, y)
     * @return Value in [0, 1]
     */
    decimal sampleImageBilinear(int imageWidth, int imageHeight,
                                const Eigen::Matrix<decimal, Eigen::Dynamic, 1>& pixels,
                                const Vec2& position) const;

    /**
     * Fits points to an ellipse and returns the least-squares error.
     *
     * @param points Rows are (x, y) points
     * @return Ellipse fit error (singular values dot coefficients)
     */
    decimal ellipseFit(const Eigen::Matrix<decimal, Eigen::Dynamic, 2>& points) const;

    /**
     * Converts an N x 2 matrix of (x, y) rows into a Points vector.
     *
     * @param M Matrix with rows (x, y)
     * @return Points vector
     */
    Points matrixToPoints(const Eigen::Matrix<decimal, Eigen::Dynamic, 2>& M) const;

    uint8_t grayThreshold_;
    int lineCount_;
    decimal lineEpsilon_;
    decimal sparseness_;
    Eigen::Matrix<decimal, Eigen::Dynamic, 1> mask_;
};

/**
 * The ZernikeEdgeDetectionAlgorithm class uses Zernike moments for sub-pixel edge detection.
 * It refines edge positions to sub-pixel accuracy by computing Zernike moments A_11 and A_20
 * in small windows around initial edge points.
 *
 * Based on: Christian (2017) "Accurate Planetary Limb Localization"
 */
class ZernikeEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
 public:
    /**
    * Constructs a new ZernikeEdgeDetectionAlgorithm
    * 
    * @param initialEdgeAlgorithm_ The algorithm to use for initial edge detection
    * @param windowSize_ Size of window around each point
    * @param transitionWidth_ Width of edge transition zone (default: 1.66)
    *
    * @pre windowSize must be a positive odd integer.
    */
    ZernikeEdgeDetectionAlgorithm(
        std::unique_ptr<EdgeDetectionAlgorithm> initialEdgeAlgorithm_,
        int windowSize_ = 7,
        decimal transitionWidth_ = DECIMAL(1.66))
        : initialEdgeAlgorithm(std::move(initialEdgeAlgorithm_)),
        windowSize(windowSize_),
        transitionWidth(transitionWidth_) {}

    // Destroys the algorithm
    virtual ~ZernikeEdgeDetectionAlgorithm() {}

    /**
     * Refines edge positions using Zernike moments sub-pixel detection.
     *
     * @param image The image of Earth
     *
     * @return The refined points on Earth's edge in image
     *
     * @throws std::invalid_argument if windowSize is not a positive odd integer
     *
     * @pre image must only contain one channel
     */
    Points Run(const Image &image) override;

 private:
    /// The initial edge detection algorithm
    std::unique_ptr<EdgeDetectionAlgorithm> initialEdgeAlgorithm;
    /// Size of window around each point (must be odd)
    int windowSize;
    /// Width of edge transition zone
    decimal transitionWidth;

    /**
     * Extracts a square window of pixels centered at the given point for Zernike moment computation.
     * Out-of-bounds coordinates are clamped to the image edges.
     *
     * @param image The image of the Earth.
     * @param center The center of the window.
     *
     * @return A newly allocated window of size windowSize * windowSize in row-major order.
     *
     * @pre image must only contain one channel
     */
    std::unique_ptr<decimal[]> extractWindow(const Image &image, const Vec2 &center);

    /**
     * Computes the Zernike polynomial convolution kernels Z_11 and Z_20 for a square unit disk 
     * mapped to the given window size.
     *
     * @return std::pair contains Z_11 and Z_20 convolution kernels
     *
     * @post Each kernel is stored in row-major order with windowSize * windowSize elements.
     */
    std::pair<std::unique_ptr<ComplexNumber[]>, std::unique_ptr<ComplexNumber[]>> computeZernikeKernels();

    /**
     * Computes the Zernike moments A_11 and A_20 for a window by convolving the window
     * with the precomputed Z_11 and Z_20 kernels.
     *
     * @param window Grayscale window data in row-major order (windowSize * windowSize elements).
     * @param kernelZ11 Z_11 kernel at each position in a windowSize * windowSize array
     * @param kernelZ20 Z_20 kernel at each position in a windowSize * windowSize array
     *
     * @return std::pair of (A_11, A_20), each represented as a ComplexNumber
     */
    std::pair<ComplexNumber, ComplexNumber> computeZernikeMoments(
        const decimal* window,
        const ComplexNumber* kernelZ11,
        const ComplexNumber* kernelZ20);

    /**
     * Extracts the edge orientation angle from the complex Zernike moment A_11.
     * The angle corresponds to the direction perpendicular to the edge in the window.
     *
     * @param A11 The complex Zernike moment A_11 (real and imaginary parts).
     *
     * @return The edge angle in radians.
     */
    decimal extractEdgeAngle(const ComplexNumber& A11);

    /**
     * Solves for the normalized edge distance l in [-1, 1] using the analytical relations
     * between A'_11, A_20, and l.
     *
     * @param A11Prime The rotated Zernike moment A_11 (aligned with the edge direction).
     * @param A20 The Zernike moment A_20 for the window.
     * @param transitionWidth The normalized transition width w of the edge model.
     *
     * @return The estimated normalized edge distance l
     */
    decimal solveEdgeDistance(decimal A11Prime, decimal A20, decimal transitionWidth);

    /**
     * Converts the refined edge position from normalized polar coordinates (l, psi) relative
     * to the window center into image pixel coordinates.
     *
     * @param windowCenter Center of the window in image coordinates (sub-pixel).
     * @param l Normalized edge distance in [-1, 1] along the edge normal.
     * @param psi Edge angle in radians.
     *
     * @return The refined edge point in image coordinates (sub-pixel).
     */
    Vec2 convertPolarToPixel(const Vec2& windowCenter, decimal l, decimal psi);
};

/**
 * Computes the groups of components within the image
 * 
 * @param image The image that defines the possible pixels
 * @param Criteria A function that accepts a pixel index and the image and returns 
 * true iff the pixel is part of the component
 * 
 * @return Components The components that are part of the image
 * 
 * @note This function iterates through each pixel in the image, but treats the image
 * as 2D, not 3D. You must program Criteria correctly to handle cases where there 
 * are multiple channels (i.e. This algorithm doesn't know how many channels are involved).
 */
Components ConnectedComponentsAlgorithm(const Image &image, std::function<bool(uint64_t, const Image &)> Criteria);

}  // namespace found

#endif  // SRC_DISTANCE_EDGE_HPP_
