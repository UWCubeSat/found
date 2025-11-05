#ifndef SRC_DISTANCE_EDGE_HPP_
#define SRC_DISTANCE_EDGE_HPP_

#include <memory>
#include <functional>
#include <vector>
#include <utility>

#include "common/style.hpp"
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
 * The ConvolutionEdgeDetectionAlgorithm class houses the Convolutional Edge Detection Algorithm.
 * This algorithm convolves and image with a kernel 
 */
class ConvolutionEdgeDetectionAlgorithm : public EdgeDetectionAlgorithm {
 public:
    /**
     * @brief Constructs a new ConvolutionEdgeDetectionAlgorithm
     * 
     * @param mask The mask to convolve with
     * @param threshold The threshold to use for detecting edges
     * 
     * @note Mask should outlive this class
     */
    ConvolutionEdgeDetectionAlgorithm(int boxBasedMaskSize, decimal eigenValueRatio,
     decimal edgeGradientRatio, decimal spacePlanetGraytoneRatio, decimal spacePlanetGradientThreshold, 
     decimal threshold) :
      boxBasedMaskSize_(boxBasedMaskSize), eigenValueRatio_(eigenValueRatio),
      edgeGradientRatio_(edgeGradientRatio), spacePlanetGraytoneRatio_(spacePlanetGraytoneRatio),
      spacePlanetGradientThreshold_(spacePlanetGradientThreshold), threshold_(threshold) {}

    /// @brief Destroys the algorithm
    virtual ~ConvolutionEdgeDetectionAlgorithm() {}

    /**
     * Provides an estimate of the edge points of Earth using
     * convolution and box-based outlier identification
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
    virtual Points Run(const Image &image) = 0;

 protected:
    /**
     * Applies the edge detection criterion to a pixel and
     * combines results across channels
     *
     * @param index The index of the pixel to check should only be channel 0
     * @param tensor The output of the convolution to check against
     * @param image The original image to check against
     * 
     * @note From using both the tensor and the original image we can use
     * more advanced techniques for noise reduction and edge detection
     * 
     * @return true if the pixel meets the criterion (is an edge), false otherwise (noise)
     */
    bool ApplyCriterion(int64_t index, const Tensor &tensor, const Image &image);

    /** 
     * Convolves the image with the mask
     * 
     * @param image The image to convolve
     * @param channel The image channel to convolve
     * 
     * @return The Tensor of the image with the mask
     * this can include decimals and negative numbers hence the new struct
     * This function clamps the edges to zero.
     * The Tensor will have the same dimensions as the image (including channels)
     *
     * @pre The mask has one channel
     */
    Tensor ConvolveWith2DMask(const Image &image, const Mask &mask, int channel);

    /**
     * Finds the magnitude of the gradient from the partials using a Euclidean norm
     * 
     * @param partials The partials to compute the gradient magnitude from
     * @param num The number of partials
     * 
     * @return The gradient magnitude Tensor
     * 
     * @pre Assumes all partials have the same width and height and that num > 0
     */
    Tensor ComputeGradientMagnitude(const Tensor partials[], int num);

    /// The index of a planet pixel
    int64_t planetIndex_;

 private:
     /**
     * Applies a Box-Based Outlier Identification to a pixel:
     * https://sites.utexas.edu/near/files/2017/04/Position-Estimation-using-Image-Dervatives.pdf
     * 
     * @param index The index of the pixel to check 
     * @param tensor The convolution output to check against
     * @param image The original image to check against
     * 
     * @pre Assumes convolution and image have the same number width, height, and number of channels. 
     * Assumes index is not out of bounds for the tensor and image.
     * 
     * @return true if the pixel meets the criterion (is an edge), false otherwise (noise)
     * 
     * @note This function operates on the channel of the provided index
     * only. If the image has multiple channels, this function should be
     * called once for each channel and the results combined appropriately. 
     */
    bool BoxBasedOutlierCriterion(int64_t index, const Tensor &tensor, const Image &image);

    /**
     * Finds the edge direction within a box centered around the pixel at index
     * 
     * @param index The index of the pixel to center the box on
     * @param tensor The data to compute the edge direction from
     * 
     * @return the unit vector representing the edge direction. The negative of this 
     * output is also a valid edge direction. If there is no direction or the direction
     * is not strong enough (lambda_min / lambda_max > eigenValueRatio), returns the zero vector
     */
    Vec2 FindEdgeDirection(int64_t index, const Tensor &tensor);

    /// The size of the box to use for box based outlier identification edge should appear straight in this box
    int boxBasedMaskSize_;
    /// The ratio of the eigenvalues must be lower than this value to indicate a direction for the edge.
    decimal eigenValueRatio_;

    /** 
     * The ratio (g_min/g_max) of the gradient of the pixels along the edge direction must be higher than this value
     * as they both supposedly belong to an edge and should share similar gradient values.
     */ 
    decimal edgeGradientRatio_;

    /** 
     * The ratio (g_min/g_max) of the graytone values orthogonal to the edge must be less than this value indicating
     * a large difference in graytone values between space and the planet. 
     */ 
    decimal spacePlanetGraytoneRatio_;

    /**
     * The gradient of the supposed space and planet pixels must be lower than this threshold. They can not also be a possible
     * edge candidate.
     */
    decimal spacePlanetGradientThreshold_;

    /// The threshold the gradient must exceed to be considered as a possible edge
    decimal threshold_;
};

/**
 * The RichardsonEdgeDetection Algorithm class houses an implementation of the Edge 
 * Detection Algorithm. This algorithm uses a 4 point central difference with a single
 * Richardson extrapolation to compute the image gradient. It then applies thresholding and 
 * box based outlier identification to return potential edge points. 
 */
class RichardsonEdgeDetectionAlgorithm : public ConvolutionEdgeDetectionAlgorithm {
 public:
    /**
     * @brief Constructs a new ConvolutionEdgeDetectionAlgorithm
     * 
     * @param mask The mask to convolve with
     * @param threshold The threshold to use for detecting edges
     * 
     * @note Mask should outlive this class
     */
    RichardsonEdgeDetectionAlgorithm(int boxBasedMaskSize, decimal eigenValueRatio = .3f,
     decimal edgeGradientRatio = .6f, decimal spacePlanetGraytoneRatio = .3f,
     decimal spacePlanetGradientThreshold = .3f, decimal threshold = 1.f) :
      ConvolutionEdgeDetectionAlgorithm(boxBasedMaskSize, eigenValueRatio,
      edgeGradientRatio, spacePlanetGraytoneRatio, spacePlanetGradientThreshold, threshold) {}

    /// @brief Destroys the algorithm
    virtual ~RichardsonEdgeDetectionAlgorithm() {}

    /**
     * Provides an estimate of the edge points of Earth using
     * convolution and box-based outlier identification
     * 
     * @param image The image of Earth
     * 
     * @return The points on Earth's edge in image
     * 
     * @pre Assumes the image has a single channel
     * 
     * @post The edge points returned are in polar order, i.e.
     * if we define the centroid of the points as P, for any
     * three consecutive points A B and C, angle APB is less than
     * angle APC
     */
    Points Run(const Image &image) override;

 private:
    //The four point central difference with a single Richardson extrapolation
    decimal mask_[9]= { 1.f/360.f, 0, 11.f/180.f, -32.f/45.f, 0, 32.f/45.f, -11.f/180.f, 0, -1.f/360.f };
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
 * Sorts points into polar clockwise order around a center point
 * Credit to DigitalLIgnote on Stack Overflow: https://stackoverflow.com/questions/6989100/sort-points-in-clockwise-order
 * 
 * @param Points The points to be sorted
 * @param center The center point to sort around
 * 
 * @pre points is not empty
 * 
 * @post points are sorted in polar clockwise order
 * 
 * @note modifies the input points. Points with the same angle will be left in their original order
 */
void PolarSort(Points &points, const Vec2 &center);

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
