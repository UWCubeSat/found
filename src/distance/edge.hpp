#ifndef SRC_DISTANCE_EDGE_HPP_
#define SRC_DISTANCE_EDGE_HPP_

#include <memory>
#include <functional>
#include <tuple>
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
 * Laplacian of Gaussian (LoC) filter to the image.
*/
class LoCEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
 public:
    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    LoCEdgeDetectionAlgorithm(/*Put more fields here!*/);

    /**
     * Place documentation here. Press enter to automatically make a new line
     * */
    virtual ~LoCEdgeDetectionAlgorithm(/*Put more fields here!*/);

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
 * Holds the real and imaginary parts of a complex Zernike moment (e.g. A_11).
 * A_20 is represented with imaginary part zero.
 */
struct ZernikeMoment {
    decimal real;
    decimal imag;
};

/**
 * The ZernikeEdgeDetectionAlgorithm class uses Zernike moments for sub-pixel edge detection.
 * It refines edge positions to sub-pixel accuracy by computing Zernike moments A_11 and A_20
 * in small windows around initial edge points (A_20 is real-only). Kernels and moments are
 * returned from internal helpers and assigned to local variables; no separate kernel or
 * moment container structs are used.
 *
 * Based on: Christian (2017) "Accurate Planetary Limb Localization"
 */
class ZernikeEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
 public:
    /**
     * @brief Constructs a new ZernikeEdgeDetectionAlgorithm
     * 
     * @param initialEdgeAlgorithm The algorithm to use for initial edge detection
     * @param windowSize Size of window around each point (must be odd, typically 7-9)
     * @param transitionWidth Width of edge transition zone (default: 1.66)
     */
    ZernikeEdgeDetectionAlgorithm(
        std::unique_ptr<EdgeDetectionAlgorithm> initialEdgeAlgorithm,
        int windowSize = 7,
        decimal transitionWidth = DECIMAL(1.66)
    ) : initialEdgeAlgorithm_(std::move(initialEdgeAlgorithm)),
        windowSize_(windowSize),
        transitionWidth_(transitionWidth) {
        // Ensure window size is odd
        if (windowSize_ % 2 == 0) {
            windowSize_++;
        }
    }

    /// @brief Destroys the algorithm
    virtual ~ZernikeEdgeDetectionAlgorithm() {}

    /**
     * Refines edge positions using Zernike moments sub-pixel detection.
     * 
     * @param image The image of Earth
     * 
     * @return The refined edge points with sub-pixel accuracy
     */
    Points Run(const Image &image) override;

 private:
    /**
     * Converts the image to grayscale in double precision for Zernike moment calculations.
     *
     * @param image The input image to convert.
     *
     * @return A newly allocated array of grayscale intensity values (one per pixel), in row-major order.
     */
    static std::unique_ptr<decimal[]> imageToGrayscaleDouble(const Image &image);

    /**
     * Extracts a square window of pixels centered at the given point for Zernike moment computation.
     * Out-of-bounds coordinates are clamped to the image edges.
     *
     * @param imageData Grayscale image data in row-major order.
     * @param imageWidth Width of the image in pixels.
     * @param imageHeight Height of the image in pixels.
     * @param centerX X-coordinate of the window center (sub-pixel).
     * @param centerY Y-coordinate of the window center (sub-pixel).
     * @param windowSize Side length of the square window (number of pixels).
     *
     * @return A newly allocated window of size windowSize * windowSize in row-major order.
     */
    static std::unique_ptr<decimal[]> extractWindow(
        const decimal* imageData,
        int imageWidth,
        int imageHeight,
        decimal centerX,
        decimal centerY,
        int windowSize
    );

    /**
     * Computes the Zernike polynomial convolution kernels Z_11 (real and imaginary) and Z_20
     * for a square unit disk mapped to the given window size. Kernels are zero outside the unit circle.
     * The Z_11 imaginary kernel is the transpose of the real kernel and is filled accordingly.
     *
     * @param windowSize Side length of the square window (kernel dimensions are windowSize x windowSize).
     *
     * @return std::tuple of (kernelZ11Real, kernelZ11Imag, kernelZ20) in row-major order.
     */
    static std::tuple<std::unique_ptr<decimal[]>, std::unique_ptr<decimal[]>, std::unique_ptr<decimal[]>>
    computeZernikeKernels(int windowSize);

    /**
     * Computes the Zernike moments A_11 and A_20 for a window by convolving the window
     * with the precomputed Z_11 and Z_20 kernels. A_20 is returned as a ZernikeMoment
     * with imaginary part zero.
     *
     * @param window Grayscale window data in row-major order (windowSize * windowSize elements).
     * @param kernelZ11Real Real part of the Z_11 kernel.
     * @param kernelZ11Imag Imaginary part of the Z_11 kernel.
     * @param kernelZ20 Z_20 kernel.
     * @param windowSize Side length of the square window.
     *
     * @return std::pair of (A_11, A_20) as ZernikeMoment; A_20.imag is 0.
     */
    static std::pair<ZernikeMoment, ZernikeMoment> computeZernikeMoments(
        const decimal* window,
        const decimal* kernelZ11Real,
        const decimal* kernelZ11Imag,
        const decimal* kernelZ20,
        int windowSize
    );

    /**
     * Extracts the edge orientation angle from the complex Zernike moment A_11.
     * The angle corresponds to the direction perpendicular to the edge in the window.
     *
     * @param A11 The complex Zernike moment A_11 (real and imaginary parts).
     *
     * @return The edge angle in radians (e.g. atan2(A11.imag, A11.real)).
     */
    static decimal extractEdgeAngle(const ZernikeMoment& A11);

    /**
     * Solves for the normalized edge distance l in [-1, 1] using the analytical relations
     * between A'_11, A_20 and l.
     *
     * @param A11Prime The rotated Zernike moment A_11 (aligned with the edge direction).
     * @param A20 The Zernike moment A_20 for the window.
     * @param transitionWidth The normalized transition width w of the edge model.
     *
     * @return The estimated normalized edge distance l
     */
    static decimal solveEdgeDistance(
        decimal A11Prime,
        decimal A20,
        decimal transitionWidth
    );

    /**
     * Converts the refined edge position from normalized polar coordinates (l, psi) relative
     * to the window center into image pixel coordinates.
     *
     * @param windowCenter Center of the window in image coordinates (sub-pixel).
     * @param l Normalized edge distance in [-1, 1] along the edge normal.
     * @param psi Edge angle in radians.
     * @param windowSize Side length of the window used for the Zernike computation.
     *
     * @return The refined edge point in image coordinates (sub-pixel).
     */
    static Vec2 convertPolarToPixel(
        const Vec2& windowCenter,
        decimal l,
        decimal psi,
        int windowSize
    );

    /// The initial edge detection algorithm
    std::unique_ptr<EdgeDetectionAlgorithm> initialEdgeAlgorithm_;
    /// Size of window around each point (must be odd)
    int windowSize_;
    /// Width of edge transition zone
    decimal transitionWidth_;
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
