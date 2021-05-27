#pragma once

#include "common.h"

namespace gl::groups {

// ConvolutionParameterEXT
enum class convolution_parameter_ext : ::libc_types::u32 {
#ifdef GL_CONVOLUTION_BORDER_MODE
    convolution_border_mode = GL_CONVOLUTION_BORDER_MODE,
#endif
#ifdef GL_CONVOLUTION_BORDER_MODE_EXT
    convolution_border_mode_ext = GL_CONVOLUTION_BORDER_MODE_EXT,
#endif
#ifdef GL_CONVOLUTION_FILTER_BIAS
    convolution_filter_bias = GL_CONVOLUTION_FILTER_BIAS,
#endif
#ifdef GL_CONVOLUTION_FILTER_BIAS_EXT
    convolution_filter_bias_ext = GL_CONVOLUTION_FILTER_BIAS_EXT,
#endif
#ifdef GL_CONVOLUTION_FILTER_SCALE
    convolution_filter_scale = GL_CONVOLUTION_FILTER_SCALE,
#endif
#ifdef GL_CONVOLUTION_FILTER_SCALE_EXT
    convolution_filter_scale_ext = GL_CONVOLUTION_FILTER_SCALE_EXT,
#endif
}; // enum class convolution_parameter_ext

} // namespace gl::groups
