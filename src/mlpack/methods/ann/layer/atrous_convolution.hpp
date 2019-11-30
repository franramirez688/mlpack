/**
 * @file atrous_convolution.hpp
 * @author Aarush Gupta
 * @author Shikhar Jaiswal
 *
 * Definition of the Atrous Convolution class.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_ANN_LAYER_ATROUS_CONVOLUTION_HPP
#define MLPACK_METHODS_ANN_LAYER_ATROUS_CONVOLUTION_HPP

#include <mlpack/prereqs.hpp>

#include <mlpack/methods/ann/convolution_rules/border_modes.hpp>
#include <mlpack/methods/ann/convolution_rules/naive_convolution.hpp>
#include <mlpack/methods/ann/convolution_rules/fft_convolution.hpp>
#include <mlpack/methods/ann/convolution_rules/svd_convolution.hpp>

#include "layer_types.hpp"
#include "padding.hpp"

namespace mlpack{
namespace ann /** Artificial Neural Network. */ {

/**
 * Implementation of the Atrous Convolution class. The Atrous Convolution
 * class represents a single layer of a neural network. Atrous (or Dilated)
 * Convolutions are just simple convolutions applied to input with the defined,
 * spaces included between the kernel cells, in order to capture a larger
 * field of reception, without having to increase dicrete kernel sizes.
 *
 * @tparam ForwardConvolutionRule Atrous Convolution to perform forward process.
 * @tparam BackwardConvolutionRule Atrous Convolution to perform backward process.
 * @tparam GradientConvolutionRule Atrous Convolution to calculate gradient.
 * @tparam InputDataType Type of the input data (arma::colvec, arma::mat,
 *         arma::sp_mat or arma::cube).
 * @tparam OutputDataType Type of the output data (arma::colvec, arma::mat,
 *         arma::sp_mat or arma::cube).
 */
template <
    typename ForwardConvolutionRule = NaiveConvolution<ValidConvolution>,
    typename BackwardConvolutionRule = NaiveConvolution<FullConvolution>,
    typename GradientConvolutionRule = NaiveConvolution<ValidConvolution>,
    typename InputDataType = arma::mat,
    typename OutputDataType = arma::mat
>
class AtrousConvolution
{
 public:
  //! Create the AtrousConvolution object.
  AtrousConvolution();

  /**
   * Create the AtrousConvolution object using the specified number of
   * input maps, output maps, filter size, stride, dilation and
   * padding parameter.
   *
   * @param inSize The number of input maps.
   * @param outSize The number of output maps.
   * @param kW Width of the filter/kernel.
   * @param kH Height of the filter/kernel.
   * @param dW Stride of filter application in the x direction.
   * @param dH Stride of filter application in the y direction.
   * @param padW Padding width of the input.
   * @param padH Padding height of the input.
   * @param inputWidth The widht of the input data.
   * @param inputHeight The height of the input data.
   * @param dilationW The space between the cells of filters in x direction.
   * @param dilationH The space between the cells of filters in y direction.
   * @param paddingType The type of padding (Valid or Same). Defaults to None.
   */
  AtrousConvolution(const size_t inSize,
                    const size_t outSize,
                    const size_t kW,
                    const size_t kH,
                    const size_t dW = 1,
                    const size_t dH = 1,
                    const size_t padW = 0,
                    const size_t padH = 0,
                    const size_t inputWidth = 0,
                    const size_t inputHeight = 0,
                    const size_t dilationW = 1,
                    const size_t dilationH = 1,
                    const std::string paddingType = "None");

  /**
   * Create the AtrousConvolution object using the specified number of
   * input maps, output maps, filter size, stride, dilation and
   * padding parameter.
   *
   * @param inSize The number of input maps.
   * @param outSize The number of output maps.
   * @param kW Width of the filter/kernel.
   * @param kH Height of the filter/kernel.
   * @param dW Stride of filter application in the x direction.
   * @param dH Stride of filter application in the y direction.
   * @param padW A two-value tuple indicating padding widths of the input.
   *             First value is padding at left side. Second value is padding on
   *             right side.
   * @param padH A two-value tuple indicating padding heights of the input.
   *             First value is padding at top. Second value is padding on
   *             bottom.
   * @param inputWidth The widht of the input data.
   * @param inputHeight The height of the input data.
   * @param dilationW The space between the cells of filters in x direction.
   * @param dilationH The space between the cells of filters in y direction.
   * @param paddingType The type of padding (Valid or Same). Defaults to None.
   */
  AtrousConvolution(const size_t inSize,
                    const size_t outSize,
                    const size_t kW,
                    const size_t kH,
                    const size_t dW,
                    const size_t dH,
                    const std::tuple<size_t, size_t> padW,
                    const std::tuple<size_t, size_t> padH,
                    const size_t inputWidth = 0,
                    const size_t inputHeight = 0,
                    const size_t dilationW = 1,
                    const size_t dilationH = 1,
                    const std::string paddingType = "None");

  /*
   * Set the weight and bias term.
   */
  void Reset();

  /**
   * Ordinary feed forward pass of a neural network, evaluating the function
   * f(x) by propagating the activity forward through f.
   *
   * @param input Input data used for evaluating the specified function.
   * @param output Resulting output activation.
   */
  template<typename eT>
  void Forward(const arma::Mat<eT>&& input, arma::Mat<eT>&& output);

  /**
   * Ordinary feed backward pass of a neural network, calculating the function
   * f(x) by propagating x backwards through f. Using the results from the feed
   * forward pass.
   *
   * @param input The propagated input activation.
   * @param gy The backpropagated error.
   * @param g The calculated gradient.
   */
  template<typename eT>
  void Backward(const arma::Mat<eT>&& /* input */,
                arma::Mat<eT>&& gy,
                arma::Mat<eT>&& g);

  /*
   * Calculate the gradient using the output delta and the input activation.
   *
   * @param input The input parameter used for calculating the gradient.
   * @param error The calculated error.
   * @param gradient The calculated gradient.
   */
  template<typename eT>
  void Gradient(const arma::Mat<eT>&& /* input */,
                arma::Mat<eT>&& error,
                arma::Mat<eT>&& gradient);

  //! Get the parameters.
  const OutputDataType& Parameters() const { return weights; }
  //! Modify the parameters.
  OutputDataType& Parameters() { return weights; }

  //! Get the output parameter.
  const OutputDataType& OutputParameter() const { return outputParameter; }
  //! Modify the output parameter.
  OutputDataType& OutputParameter() { return outputParameter; }

  //! Get the delta.
  const OutputDataType& Delta() const { return delta; }
  //! Modify the delta.
  OutputDataType& Delta() { return delta; }

  //! Get the gradient.
  const OutputDataType& Gradient() const { return gradient; }
  //! Modify the gradient.
  OutputDataType& Gradient() { return gradient; }

  //! Get the input width.
  const size_t& InputWidth() const { return inputWidth; }
  //! Modify input the width.
  size_t& InputWidth() { return inputWidth; }

  //! Get the input height.
  const size_t& InputHeight() const { return inputHeight; }
  //! Modify the input height.
  size_t& InputHeight() { return inputHeight; }

  //! Get the output width.
  const size_t& OutputWidth() const { return outputWidth; }
  //! Modify the output width.
  size_t& OutputWidth() { return outputWidth; }

  //! Get the output height.
  const size_t& OutputHeight() const { return outputHeight; }
  //! Modify the output height.
  size_t& OutputHeight() { return outputHeight; }

  //! Get the input size
  const size_t& InputSize() const { return inSize; }

  //! Get the output size.
  const size_t& OutputSize() const { return outSize; }

  //! Get the kernel size (width and height).
  const std::tuple<size_t, size_t> KernelSize() const
  {
    return std::forward_as_tuple(kW, kH);
  }
  //! Modify the kernel size (width and height).
  std::tuple<size_t&, size_t&> KernelSize()
  {
    return std::forward_as_tuple(kW, kH);
  }

  //! Get the stride dimensions (width and height).
  const std::tuple<size_t, size_t> Strides() const
  {
    return std::forward_as_tuple(dW, dH);
  }
  //! Modify the stride dimensions (width and height).
  std::tuple<size_t&, size_t&> Strides()
  {
    return std::forward_as_tuple(dW, dH);
  }

  //! Get the padding height dimensions (top and bottom).
  const std::tuple<size_t, size_t> PadHSize() const
  {
    return std::forward_as_tuple(padHTop, padHBottom);
  }
  //! Modify the padding dimensions (top and bottom).
  std::tuple<size_t&, size_t&> PadHSize()
  {
    return std::forward_as_tuple(padHTop, padHBottom);
  }

  //! Get the padding width dimensions (left and right).
  const std::tuple<size_t, size_t> PadWSize() const
  {
    return std::forward_as_tuple(padWLeft, padWRight);
  }
  //! Modify the padding width dimensions (left and right).
  std::tuple<size_t&, size_t&> PadWSize()
  {
    return std::forward_as_tuple(padWLeft, padWRight);
  }

  //! Get the dilation rate (width and height).
  const std::tuple<size_t, size_t> DilationRate() const
  {
    return std::forward_as_tuple(dilationW, dilationH);
  }
  //! Modify the dilation rate (width and height).
  std::tuple<size_t&, size_t&> DilationRate()
  {
    return std::forward_as_tuple(dilationW, dilationH);
  }

  //! Modify the bias weights of the layer.
  arma::mat& Bias() { return bias; }

  /**
   * Serialize the layer
   */
  template<typename Archive>
  void serialize(Archive& ar, const unsigned int /* version */);

 private:
  /*
   * Return the convolution output size.
   *
   * @param size The size of the input (row or column).
   * @param k The size of the filter (width or height).
   * @param s The stride size (x or y direction).
   * @param pSideOne The size of the padding (width or height) on one side.
   * @param pSideTwo The size of the padding (width or height) on another side.
   * @param d The dilation size.
   * @return The convolution output size.
   */
  size_t ConvOutSize(const size_t size,
                     const size_t k,
                     const size_t s,
                     const size_t pSideOne,
                     const size_t pSideTwo,
                     const size_t d)
  {
    return std::floor(size + pSideOne + pSideTwo - d * (k - 1) - 1) / s + 1;
  }

  /*
   * Function to assign padding such that output size is same as input size.
   */
  void InitializeSamePadding();

  /*
   * Rotates a 3rd-order tensor counterclockwise by 180 degrees.
   *
   * @param input The input data to be rotated.
   * @param output The rotated output.
   */
  template<typename eT>
  void Rotate180(const arma::Cube<eT>& input, arma::Cube<eT>& output)
  {
    output = arma::Cube<eT>(input.n_rows, input.n_cols, input.n_slices);

    // * left-right flip, up-down flip */
    for (size_t s = 0; s < output.n_slices; s++)
      output.slice(s) = arma::fliplr(arma::flipud(input.slice(s)));
  }

  /*
   * Rotates a dense matrix counterclockwise by 180 degrees.
   *
   * @param input The input data to be rotated.
   * @param output The rotated output.
   */
  template<typename eT>
  void Rotate180(const arma::Mat<eT>& input, arma::Mat<eT>& output)
  {
    // * left-right flip, up-down flip */
    output = arma::fliplr(arma::flipud(input));
  }

  //! Locally-stored number of input channels.
  size_t inSize;

  //! Locally-stored number of output channels.
  size_t outSize;

  //! Locally-stored number of input units.
  size_t batchSize;

  //! Locally-stored filter/kernel width.
  size_t kW;

  //! Locally-stored filter/kernel height.
  size_t kH;

  //! Locally-stored stride of the filter in x-direction.
  size_t dW;

  //! Locally-stored stride of the filter in y-direction.
  size_t dH;

  //! Locally-stored left-side padding width.
  size_t padWLeft;

  //! Locally-stored right-side padding width.
  size_t padWRight;

  //! Locally-stored bottom padding height.
  size_t padHBottom;

  //! Locally-stored top padding height.
  size_t padHTop;

  //! Locally-stored weight object.
  OutputDataType weights;

  //! Locally-stored weight object.
  arma::cube weight;

  //! Locally-stored bias term object.
  arma::mat bias;

  //! Locally-stored input width.
  size_t inputWidth;

  //! Locally-stored input height.
  size_t inputHeight;

  //! Locally-stored output width.
  size_t outputWidth;

  //! Locally-stored output height.
  size_t outputHeight;

  //! Locally-stored width dilation factor.
  size_t dilationW;

  //! Locally-stored height dilation factor.
  size_t dilationH;

  //! Locally-stored transformed output parameter.
  arma::cube outputTemp;

  //! Locally-stored transformed input parameter.
  arma::cube inputTemp;

  //! Locally-stored transformed padded input parameter.
  arma::cube inputPaddedTemp;

  //! Locally-stored transformed error parameter.
  arma::cube gTemp;

  //! Locally-stored transformed gradient parameter.
  arma::cube gradientTemp;

  //! Locally-stored padding layer.
  Padding<>* padding;

  //! Locally-stored delta object.
  OutputDataType delta;

  //! Locally-stored gradient object.
  OutputDataType gradient;

  //! Locally-stored output parameter object.
  OutputDataType outputParameter;
}; // class AtrousConvolution

} // namespace ann
} // namespace mlpack

//! Set the serialization version of the AtrousConvolution class.
namespace boost {
namespace serialization {

template<
    typename ForwardConvolutionRule,
    typename BackwardConvolutionRule,
    typename GradientConvolutionRule,
    typename InputDataType,
    typename OutputDataType
>
struct version<
    mlpack::ann::AtrousConvolution<ForwardConvolutionRule,
        BackwardConvolutionRule, GradientConvolutionRule, InputDataType,
        OutputDataType> >
{
  BOOST_STATIC_CONSTANT(int, value = 1);
};

} // namespace serialization
} // namespace boost

// Include implementation
#include "atrous_convolution_impl.hpp"

#endif
