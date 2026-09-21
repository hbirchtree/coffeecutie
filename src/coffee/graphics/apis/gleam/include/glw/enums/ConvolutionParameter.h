#pragma once

#include "common.h"

namespace gl::group {

// ConvolutionParameter
enum class convolution_parameter : u32
{
    convolution_border_color     = 0x8154, // GL_CONVOLUTION_BORDER_COLOR
    convolution_border_mode      = 0x8013, // GL_CONVOLUTION_BORDER_MODE
    convolution_filter_bias      = 0x8015, // GL_CONVOLUTION_FILTER_BIAS
    convolution_filter_scale     = 0x8014, // GL_CONVOLUTION_FILTER_SCALE
    convolution_format           = 0x8017, // GL_CONVOLUTION_FORMAT
    convolution_height           = 0x8019, // GL_CONVOLUTION_HEIGHT
    convolution_width            = 0x8018, // GL_CONVOLUTION_WIDTH
    max_convolution_height       = 0x801B, // GL_MAX_CONVOLUTION_HEIGHT
    max_convolution_width        = 0x801A, // GL_MAX_CONVOLUTION_WIDTH
    convolution_border_mode_ext  = 0x8013, // GL_CONVOLUTION_BORDER_MODE_EXT
    convolution_filter_bias_ext  = 0x8015, // GL_CONVOLUTION_FILTER_BIAS_EXT
    convolution_filter_scale_ext = 0x8014, // GL_CONVOLUTION_FILTER_SCALE_EXT
    convolution_format_ext       = 0x8017, // GL_CONVOLUTION_FORMAT_EXT
    convolution_height_ext       = 0x8019, // GL_CONVOLUTION_HEIGHT_EXT
    convolution_width_ext        = 0x8018, // GL_CONVOLUTION_WIDTH_EXT
    max_convolution_height_ext   = 0x801B, // GL_MAX_CONVOLUTION_HEIGHT_EXT
    max_convolution_width_ext    = 0x801A, // GL_MAX_CONVOLUTION_WIDTH_EXT
}; // enum class convolution_parameter

} // namespace gl::group
