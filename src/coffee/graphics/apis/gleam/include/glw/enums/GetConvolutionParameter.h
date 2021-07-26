#pragma once

#include "common.h"

namespace gl::group {

// GetConvolutionParameter
enum class get_convolution_parameter : ::libc_types::u32
{
#ifdef GL_CONVOLUTION_BORDER_COLOR
    convolution_border_color = GL_CONVOLUTION_BORDER_COLOR,
#endif
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
#ifdef GL_CONVOLUTION_FORMAT
    convolution_format = GL_CONVOLUTION_FORMAT,
#endif
#ifdef GL_CONVOLUTION_FORMAT_EXT
    convolution_format_ext = GL_CONVOLUTION_FORMAT_EXT,
#endif
#ifdef GL_CONVOLUTION_HEIGHT
    convolution_height = GL_CONVOLUTION_HEIGHT,
#endif
#ifdef GL_CONVOLUTION_HEIGHT_EXT
    convolution_height_ext = GL_CONVOLUTION_HEIGHT_EXT,
#endif
#ifdef GL_CONVOLUTION_WIDTH
    convolution_width = GL_CONVOLUTION_WIDTH,
#endif
#ifdef GL_CONVOLUTION_WIDTH_EXT
    convolution_width_ext = GL_CONVOLUTION_WIDTH_EXT,
#endif
#ifdef GL_MAX_CONVOLUTION_HEIGHT
    max_convolution_height = GL_MAX_CONVOLUTION_HEIGHT,
#endif
#ifdef GL_MAX_CONVOLUTION_HEIGHT_EXT
    max_convolution_height_ext = GL_MAX_CONVOLUTION_HEIGHT_EXT,
#endif
#ifdef GL_MAX_CONVOLUTION_WIDTH
    max_convolution_width = GL_MAX_CONVOLUTION_WIDTH,
#endif
#ifdef GL_MAX_CONVOLUTION_WIDTH_EXT
    max_convolution_width_ext = GL_MAX_CONVOLUTION_WIDTH_EXT,
#endif
}; // enum class get_convolution_parameter

} // namespace gl::group
