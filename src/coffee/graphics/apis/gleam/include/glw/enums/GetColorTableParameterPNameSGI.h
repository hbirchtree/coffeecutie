#pragma once

#include "common.h"

namespace gl::groups {

// GetColorTableParameterPNameSGI
enum class get_color_table_parameter_prop_sgi : ::libc_types::u32 {
#ifdef GL_COLOR_TABLE_ALPHA_SIZE
    color_table_alpha_size = GL_COLOR_TABLE_ALPHA_SIZE,
#endif
#ifdef GL_COLOR_TABLE_ALPHA_SIZE_SGI
    color_table_alpha_size_sgi = GL_COLOR_TABLE_ALPHA_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_BIAS
    color_table_bias = GL_COLOR_TABLE_BIAS,
#endif
#ifdef GL_COLOR_TABLE_BIAS_SGI
    color_table_bias_sgi = GL_COLOR_TABLE_BIAS_SGI,
#endif
#ifdef GL_COLOR_TABLE_BLUE_SIZE
    color_table_blue_size = GL_COLOR_TABLE_BLUE_SIZE,
#endif
#ifdef GL_COLOR_TABLE_BLUE_SIZE_SGI
    color_table_blue_size_sgi = GL_COLOR_TABLE_BLUE_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_FORMAT
    color_table_format = GL_COLOR_TABLE_FORMAT,
#endif
#ifdef GL_COLOR_TABLE_FORMAT_SGI
    color_table_format_sgi = GL_COLOR_TABLE_FORMAT_SGI,
#endif
#ifdef GL_COLOR_TABLE_GREEN_SIZE
    color_table_green_size = GL_COLOR_TABLE_GREEN_SIZE,
#endif
#ifdef GL_COLOR_TABLE_GREEN_SIZE_SGI
    color_table_green_size_sgi = GL_COLOR_TABLE_GREEN_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_INTENSITY_SIZE
    color_table_intensity_size = GL_COLOR_TABLE_INTENSITY_SIZE,
#endif
#ifdef GL_COLOR_TABLE_INTENSITY_SIZE_SGI
    color_table_intensity_size_sgi = GL_COLOR_TABLE_INTENSITY_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_LUMINANCE_SIZE
    color_table_luminance_size = GL_COLOR_TABLE_LUMINANCE_SIZE,
#endif
#ifdef GL_COLOR_TABLE_LUMINANCE_SIZE_SGI
    color_table_luminance_size_sgi = GL_COLOR_TABLE_LUMINANCE_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_RED_SIZE
    color_table_red_size = GL_COLOR_TABLE_RED_SIZE,
#endif
#ifdef GL_COLOR_TABLE_RED_SIZE_SGI
    color_table_red_size_sgi = GL_COLOR_TABLE_RED_SIZE_SGI,
#endif
#ifdef GL_COLOR_TABLE_SCALE
    color_table_scale = GL_COLOR_TABLE_SCALE,
#endif
#ifdef GL_COLOR_TABLE_SCALE_SGI
    color_table_scale_sgi = GL_COLOR_TABLE_SCALE_SGI,
#endif
#ifdef GL_COLOR_TABLE_WIDTH
    color_table_width = GL_COLOR_TABLE_WIDTH,
#endif
#ifdef GL_COLOR_TABLE_WIDTH_SGI
    color_table_width_sgi = GL_COLOR_TABLE_WIDTH_SGI,
#endif
}; // enum class get_color_table_parameter_prop_sgi

} // namespace gl::groups
