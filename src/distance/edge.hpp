#ifndef SRC_DISTANCE_EDGE_HPP_
#define SRC_DISTANCE_EDGE_HPP_

#include <functional>
#include <memory>
#include <utility>
#include <vector>

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
 * The ZernikeEdgeDetectionAlgorithm class ingests a pixel level edge detection algorithm to
 * obtain a pixel level approximation of the edge of earth and then performs a sub-pixel edge
 * correction using the moment based edge-orientation and offset.
 *
 * Based on: Christian (2017) "Accurate Planetary Limb Localization"
 * https://www.researchgate.net/publication/317742756_Accurate_Planetary_
 * Limb_Localization_for_Image-Based_Spacecraft_Navigation
 */
class ZernikeEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
 public:
    /**
    * Constructs a new ZernikeEdgeDetectionAlgorithm
    * 
    * @param initialEdgeAlgorithm The algorithm to use for initial pixel level edge detection
    * @param maskSize_ Size of the square mask around each point
    * @param transitionWidth_ Width of edge transition zone (default: 1.66)
    *
    * @pre maskSize_ must be a positive odd integer.
    * @pre transitionWidth_ must be greater than 0
    */
    ZernikeEdgeDetectionAlgorithm(
        EdgeDetectionAlgorithm &initialEdgeAlgorithm,
        int maskSize = 7,
        decimal transitionWidth = DECIMAL(1.66))
        : initialEdgeAlgorithm_(initialEdgeAlgorithm),
          maskSize_(maskSize),
          transitionWidth_(transitionWidth) {}

    // Destroys the algorithm
    virtual ~ZernikeEdgeDetectionAlgorithm() {}

    /**
     * Ingests a pixel level edge detection algorithm to obtain a pixel level
     * approximation of the edge of earth and then performs a sub-pixel edge
     * correction using the moment based edge-orientation and offset.
     *
     * @param image The image of Earth
     *
     * @return The refined points on Earth's edge in image
     *
     * @throws std::invalid_argument if maskSize_ is not a positive odd integer
     *
     * @pre image must only contain one channel
     */
    Points Run(const Image &image) override;

    /**
     * Computes the Zernike polynomial convolution kernels M_11 and M_20 for a square unit disk 
     * mapped to the given mask size.
     *
     * @return std::pair of M_11 and M_20 convolution kernels (row-major, maskSize_² elements each)
     */
    std::pair<std::unique_ptr<ComplexNumber[]>, std::unique_ptr<ComplexNumber[]>> computeZernikeKernels();

    /**
     * Extracts a square mask of pixels centered at the given point for Zernike moment computation.
     *
     * @param image The image of the Earth.
     * @param center The center of the mask.
     * @param kernelM11 M_11 kernel in row-major order (maskSize_ * maskSize_).
     * @param kernelM20 M_20 kernel in row-major order (maskSize_ * maskSize_).
     *
     * @return a pair of ComplexNumbers (A_11, A_20)
     */
    std::pair<ComplexNumber, ComplexNumber> computeZernikeMoments(
        const Image &image,
        const Vec2<int> &center,
        const std::unique_ptr<ComplexNumber[]> &kernelM11,
        const std::unique_ptr<ComplexNumber[]> &kernelM20);

    /**
     * Extracts the edge orientation angle from the complex Zernike moment A_11.
     * The angle corresponds to the direction perpendicular to the edge in the mask.
     *
     * @param A11 The complex Zernike moment A_11 (real and imaginary parts).
     *
     * @return The edge angle in radians.
     */
    decimal extractEdgeAngle(const ComplexNumber &A11);

    /**
     * Solves for the normalized edge distance l in [-1, 1] using the analytical relations
     * between A'_11, A_20, and l.
     *
     * @param A11Prime The rotated Zernike moment A_11 (aligned with the edge direction).
     * @param A20 The Zernike moment A_20 for the mask.
     *
     * @return The estimated normalized edge distance l
     */
    decimal extractEdgeOffset(decimal A11Prime, decimal A20);

    /**
     * Converts the refined edge position from normalized polar coordinates (l, psi) relative
     * to the mask center into image pixel coordinates.
     *
     * @param maskCenter Center of the mask in image coordinates (sub-pixel).
     * @param l Normalized edge distance in [-1, 1] along the edge normal.
     * @param psi Edge angle in radians.
     *
     * @return The refined edge point in image coordinates (sub-pixel).
     */
    Vec2<int> applyEdgeCorrection(const Vec2<int>& maskCenter, decimal l, decimal psi);

 private:
    /// The initial edge detection algorithm
    EdgeDetectionAlgorithm& initialEdgeAlgorithm_;
    /// Size of the square mask around each point (must be odd)
    int maskSize_;
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
