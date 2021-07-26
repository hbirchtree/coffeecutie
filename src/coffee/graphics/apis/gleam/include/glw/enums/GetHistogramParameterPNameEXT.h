#pragma once

#include "common.h"

namespace gl::group {

// GetHistogramParameterPNameEXT
enum class get_histogram_parameter_prop_ext : ::libc_types::u32
{
#ifdef GL_HISTOGRAM_ALPHA_SIZE
    histogram_alpha_size = GL_HISTOGRAM_ALPHA_SIZE,
#endif
#ifdef GL_HISTOGRAM_ALPHA_SIZE_EXT
    histogram_alpha_size_ext = GL_HISTOGRAM_ALPHA_SIZE_EXT,
#endif
#ifdef GL_HISTOGRAM_BLUE_SIZE
    histogram_blue_size = GL_HISTOGRAM_BLUE_SIZE,
#endif
#ifdef GL_HISTOGRAM_BLUE_SIZE_EXT
    histogram_blue_size_ext = GL_HISTOGRAM_BLUE_SIZE_EXT,
#endif
#ifdef GL_HISTOGRAM_FORMAT
    histogram_format = GL_HISTOGRAM_FORMAT,
#endif
#ifdef GL_HISTOGRAM_FORMAT_EXT
    histogram_format_ext = GL_HISTOGRAM_FORMAT_EXT,
#endif
#ifdef GL_HISTOGRAM_GREEN_SIZE
    histogram_green_size = GL_HISTOGRAM_GREEN_SIZE,
#endif
#ifdef GL_HISTOGRAM_GREEN_SIZE_EXT
    histogram_green_size_ext = GL_HISTOGRAM_GREEN_SIZE_EXT,
#endif
#ifdef GL_HISTOGRAM_LUMINANCE_SIZE
    histogram_luminance_size = GL_HISTOGRAM_LUMINANCE_SIZE,
#endif
#ifdef GL_HISTOGRAM_LUMINANCE_SIZE_EXT
    histogram_luminance_size_ext = GL_HISTOGRAM_LUMINANCE_SIZE_EXT,
#endif
#ifdef GL_HISTOGRAM_RED_SIZE
    histogram_red_size = GL_HISTOGRAM_RED_SIZE,
#endif
#ifdef GL_HISTOGRAM_RED_SIZE_EXT
    histogram_red_size_ext = GL_HISTOGRAM_RED_SIZE_EXT,
#endif
#ifdef GL_HISTOGRAM_SINK
    histogram_sink = GL_HISTOGRAM_SINK,
#endif
#ifdef GL_HISTOGRAM_SINK_EXT
    histogram_sink_ext = GL_HISTOGRAM_SINK_EXT,
#endif
#ifdef GL_HISTOGRAM_WIDTH
    histogram_width = GL_HISTOGRAM_WIDTH,
#endif
#ifdef GL_HISTOGRAM_WIDTH_EXT
    histogram_width_ext = GL_HISTOGRAM_WIDTH_EXT,
#endif
}; // enum class get_histogram_parameter_prop_ext

} // namespace gl::group
