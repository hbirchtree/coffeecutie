#pragma once

#include "common.h"

namespace gl::group {

// ColorTableParameterPName
enum class color_table_parameter_prop : u32
{
    color_table_alpha_size     = 0x80DD, // GL_COLOR_TABLE_ALPHA_SIZE
    color_table_bias           = 0x80D7, // GL_COLOR_TABLE_BIAS
    color_table_blue_size      = 0x80DC, // GL_COLOR_TABLE_BLUE_SIZE
    color_table_format         = 0x80D8, // GL_COLOR_TABLE_FORMAT
    color_table_green_size     = 0x80DB, // GL_COLOR_TABLE_GREEN_SIZE
    color_table_intensity_size = 0x80DF, // GL_COLOR_TABLE_INTENSITY_SIZE
    color_table_luminance_size = 0x80DE, // GL_COLOR_TABLE_LUMINANCE_SIZE
    color_table_red_size       = 0x80DA, // GL_COLOR_TABLE_RED_SIZE
    color_table_scale          = 0x80D6, // GL_COLOR_TABLE_SCALE
    color_table_width          = 0x80D9, // GL_COLOR_TABLE_WIDTH
    color_table_alpha_size_sgi = 0x80DD, // GL_COLOR_TABLE_ALPHA_SIZE_SGI
    color_table_bias_sgi       = 0x80D7, // GL_COLOR_TABLE_BIAS_SGI
    color_table_blue_size_sgi  = 0x80DC, // GL_COLOR_TABLE_BLUE_SIZE_SGI
    color_table_format_sgi     = 0x80D8, // GL_COLOR_TABLE_FORMAT_SGI
    color_table_green_size_sgi = 0x80DB, // GL_COLOR_TABLE_GREEN_SIZE_SGI
    color_table_intensity_size_sgi =
        0x80DF, // GL_COLOR_TABLE_INTENSITY_SIZE_SGI
    color_table_luminance_size_sgi =
        0x80DE,                        // GL_COLOR_TABLE_LUMINANCE_SIZE_SGI
    color_table_red_size_sgi = 0x80DA, // GL_COLOR_TABLE_RED_SIZE_SGI
    color_table_scale_sgi    = 0x80D6, // GL_COLOR_TABLE_SCALE_SGI
    color_table_width_sgi    = 0x80D9, // GL_COLOR_TABLE_WIDTH_SGI
}; // enum class color_table_parameter_prop

} // namespace gl::group
