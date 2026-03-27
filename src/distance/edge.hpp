#ifndef SRC_DISTANCE_EDGE_HPP_
#define SRC_DISTANCE_EDGE_HPP_

#include <memory>
#include <functional>
#include <utility>

#include <Eigen/Core>
#include <unsupported/Eigen/CXX11/Tensor>

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
 * Base class for kernel-based edge detection with optimized 3x3 convolution
 * (im2col + GEMM).
 */
class KernelEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
 protected:
    /**
     * Optimized 3x3 convolution via im2col + GEMM.
     *
     * @param gray Grayscale image (rows x cols), normalized [0, 1]
     * @param kernel Flattened 3x3 kernel as Tensor of shape (9, 1), row-major order
     * @return Convolved output of size (rows - 2, cols - 2)
     */
    Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> Convolve3x3(
        const Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> &gray,
        const Eigen::Tensor<decimal, 2> &kernel) const;
};

/**
 * The SobelEdgeDetectionAlgorithm class finds horizon edge points using Sobel gradients,
 * non-maximum suppression, and hysteresis thresholding.
 */
class SobelEdgeDetectionAlgorithm : public KernelEdgeDetectionAlgorithm {
 public:
    /**
     * Constructs a new SobelEdgeDetectionAlgorithm.
     *
     * @param highThreshold Threshold for strong edges in hysteresis (e.g. 0.1–0.3 for
     *        normalized [0,1] grayscale)
     */
    explicit SobelEdgeDetectionAlgorithm(decimal highThreshold);

    virtual ~SobelEdgeDetectionAlgorithm() {}

    /**
     * Detects edge points via Sobel -> NMS -> Hysteresis.
     *
     * @param image Grayscale or multi-channel image (uses first channel if multi-channel)
     *
     * @return Points on the detected edges (pixel coordinates)
     */
    Points Run(const Image &image) override;

 private:
    Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> ToGrayscaleMatrix(
        const Image &image) const;

    void ComputeMagnitudeAndDirection(
        const Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> &gx,
        const Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> &gy,
        Eigen::Array<decimal, Eigen::Dynamic, Eigen::Dynamic> *mag,
        Eigen::Array<decimal, Eigen::Dynamic, Eigen::Dynamic> *ratio) const;

    Eigen::Array<bool, Eigen::Dynamic, Eigen::Dynamic> NonMaxSuppression(
        const Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> &gx,
        const Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> &gy,
        const Eigen::Array<decimal, Eigen::Dynamic, Eigen::Dynamic> &mag,
        const Eigen::Array<decimal, Eigen::Dynamic, Eigen::Dynamic> &ratio) const;

    Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> HysteresisThreshold(
        const Eigen::Array<bool, Eigen::Dynamic, Eigen::Dynamic> &is_max,
        const Eigen::Array<decimal, Eigen::Dynamic, Eigen::Dynamic> &mag) const;

    Points CollectPointsAndSortPolar(
        const Eigen::Matrix<decimal, Eigen::Dynamic, Eigen::Dynamic> &nms_result) const;

    decimal highThreshold_;
    Eigen::Tensor<decimal, 2> gxKernel_;
    Eigen::Tensor<decimal, 2> gyKernel_;
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
     * Computes the Zernike polynomial convolution kernels as a single matrix for efficient
     * moment computation. Columns are [Z_11 real, Z_11 imag, Z_20 real].
     *
     * @return Matrix of size (windowSize^2, 3) in column-major order for one K^T * w multiply
     */
    Eigen::Matrix<decimal, Eigen::Dynamic, 3> computeZernikeKernels();

    /**
     * Computes the Zernike moments A_11 and A_20 via one matrix-vector multiply (K^T * window).
     *
     * @param window Grayscale window data in row-major order (windowSize * windowSize elements).
     * @param kernelMatrix Matrix from computeZernikeKernels(), size (windowSize^2, 3)
     *
     * @return std::pair of (A_11, A_20), each represented as a ComplexNumber
     */
    std::pair<ComplexNumber, ComplexNumber> computeZernikeMoments(
        const decimal* window,
        const Eigen::Matrix<decimal, Eigen::Dynamic, 3> &kernelMatrix);

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
     * Computes edge strength from A_11 magnitude for weak-edge rejection.
     *
     * @param A11 The complex Zernike moment A_11.
     *
     * @return Non-negative edge strength.
     */
    decimal computeEdgeStrength(const ComplexNumber& A11);

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
