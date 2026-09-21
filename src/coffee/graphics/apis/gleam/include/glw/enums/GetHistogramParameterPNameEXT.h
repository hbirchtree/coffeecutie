#pragma once

#include "common.h"

namespace gl::group {

// GetHistogramParameterPNameEXT
enum class get_histogram_parameter_prop_ext : u32
{
    histogram_alpha_size         = 0x802B, // GL_HISTOGRAM_ALPHA_SIZE
    histogram_blue_size          = 0x802A, // GL_HISTOGRAM_BLUE_SIZE
    histogram_format             = 0x8027, // GL_HISTOGRAM_FORMAT
    histogram_green_size         = 0x8029, // GL_HISTOGRAM_GREEN_SIZE
    histogram_luminance_size     = 0x802C, // GL_HISTOGRAM_LUMINANCE_SIZE
    histogram_red_size           = 0x8028, // GL_HISTOGRAM_RED_SIZE
    histogram_sink               = 0x802D, // GL_HISTOGRAM_SINK
    histogram_width              = 0x8026, // GL_HISTOGRAM_WIDTH
    histogram_alpha_size_ext     = 0x802B, // GL_HISTOGRAM_ALPHA_SIZE_EXT
    histogram_blue_size_ext      = 0x802A, // GL_HISTOGRAM_BLUE_SIZE_EXT
    histogram_format_ext         = 0x8027, // GL_HISTOGRAM_FORMAT_EXT
    histogram_green_size_ext     = 0x8029, // GL_HISTOGRAM_GREEN_SIZE_EXT
    histogram_luminance_size_ext = 0x802C, // GL_HISTOGRAM_LUMINANCE_SIZE_EXT
    histogram_red_size_ext       = 0x8028, // GL_HISTOGRAM_RED_SIZE_EXT
    histogram_sink_ext           = 0x802D, // GL_HISTOGRAM_SINK_EXT
    histogram_width_ext          = 0x8026, // GL_HISTOGRAM_WIDTH_EXT
}; // enum class get_histogram_parameter_prop_ext

} // namespace gl::group
