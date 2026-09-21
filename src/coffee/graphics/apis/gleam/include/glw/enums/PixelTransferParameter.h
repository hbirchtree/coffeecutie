#pragma once

#include "common.h"

namespace gl::group {

// PixelTransferParameter
enum class pixel_transfer_parameter : u32
{
    alpha_bias                    = 0x0D1D, // GL_ALPHA_BIAS
    alpha_scale                   = 0x0D1C, // GL_ALPHA_SCALE
    blue_bias                     = 0x0D1B, // GL_BLUE_BIAS
    blue_scale                    = 0x0D1A, // GL_BLUE_SCALE
    depth_bias                    = 0x0D1F, // GL_DEPTH_BIAS
    depth_scale                   = 0x0D1E, // GL_DEPTH_SCALE
    green_bias                    = 0x0D19, // GL_GREEN_BIAS
    green_scale                   = 0x0D18, // GL_GREEN_SCALE
    index_offset                  = 0x0D13, // GL_INDEX_OFFSET
    index_shift                   = 0x0D12, // GL_INDEX_SHIFT
    map_color                     = 0x0D10, // GL_MAP_COLOR
    map_stencil                   = 0x0D11, // GL_MAP_STENCIL
    post_color_matrix_alpha_bias  = 0x80BB, // GL_POST_COLOR_MATRIX_ALPHA_BIAS
    post_color_matrix_alpha_scale = 0x80B7, // GL_POST_COLOR_MATRIX_ALPHA_SCALE
    post_color_matrix_blue_bias   = 0x80BA, // GL_POST_COLOR_MATRIX_BLUE_BIAS
    post_color_matrix_blue_scale  = 0x80B6, // GL_POST_COLOR_MATRIX_BLUE_SCALE
    post_color_matrix_green_bias  = 0x80B9, // GL_POST_COLOR_MATRIX_GREEN_BIAS
    post_color_matrix_green_scale = 0x80B5, // GL_POST_COLOR_MATRIX_GREEN_SCALE
    post_color_matrix_red_bias    = 0x80B8, // GL_POST_COLOR_MATRIX_RED_BIAS
    post_color_matrix_red_scale   = 0x80B4, // GL_POST_COLOR_MATRIX_RED_SCALE
    post_convolution_alpha_bias   = 0x8023, // GL_POST_CONVOLUTION_ALPHA_BIAS
    post_convolution_alpha_scale  = 0x801F, // GL_POST_CONVOLUTION_ALPHA_SCALE
    post_convolution_blue_bias    = 0x8022, // GL_POST_CONVOLUTION_BLUE_BIAS
    post_convolution_blue_scale   = 0x801E, // GL_POST_CONVOLUTION_BLUE_SCALE
    post_convolution_green_bias   = 0x8021, // GL_POST_CONVOLUTION_GREEN_BIAS
    post_convolution_green_scale  = 0x801D, // GL_POST_CONVOLUTION_GREEN_SCALE
    post_convolution_red_bias     = 0x8020, // GL_POST_CONVOLUTION_RED_BIAS
    post_convolution_red_scale    = 0x801C, // GL_POST_CONVOLUTION_RED_SCALE
    red_bias                      = 0x0D15, // GL_RED_BIAS
    red_scale                     = 0x0D14, // GL_RED_SCALE
    post_color_matrix_alpha_bias_sgi =
        0x80BB, // GL_POST_COLOR_MATRIX_ALPHA_BIAS_SGI
    post_color_matrix_alpha_scale_sgi =
        0x80B7, // GL_POST_COLOR_MATRIX_ALPHA_SCALE_SGI
    post_color_matrix_blue_bias_sgi =
        0x80BA, // GL_POST_COLOR_MATRIX_BLUE_BIAS_SGI
    post_color_matrix_blue_scale_sgi =
        0x80B6, // GL_POST_COLOR_MATRIX_BLUE_SCALE_SGI
    post_color_matrix_green_bias_sgi =
        0x80B9, // GL_POST_COLOR_MATRIX_GREEN_BIAS_SGI
    post_color_matrix_green_scale_sgi =
        0x80B5, // GL_POST_COLOR_MATRIX_GREEN_SCALE_SGI
    post_color_matrix_red_bias_sgi =
        0x80B8, // GL_POST_COLOR_MATRIX_RED_BIAS_SGI
    post_color_matrix_red_scale_sgi =
        0x80B4, // GL_POST_COLOR_MATRIX_RED_SCALE_SGI
    post_convolution_alpha_bias_ext =
        0x8023, // GL_POST_CONVOLUTION_ALPHA_BIAS_EXT
    post_convolution_alpha_scale_ext =
        0x801F, // GL_POST_CONVOLUTION_ALPHA_SCALE_EXT
    post_convolution_blue_bias_ext =
        0x8022, // GL_POST_CONVOLUTION_BLUE_BIAS_EXT
    post_convolution_blue_scale_ext =
        0x801E, // GL_POST_CONVOLUTION_BLUE_SCALE_EXT
    post_convolution_green_bias_ext =
        0x8021, // GL_POST_CONVOLUTION_GREEN_BIAS_EXT
    post_convolution_green_scale_ext =
        0x801D, // GL_POST_CONVOLUTION_GREEN_SCALE_EXT
    post_convolution_red_bias_ext = 0x8020, // GL_POST_CONVOLUTION_RED_BIAS_EXT
    post_convolution_red_scale_ext =
        0x801C, // GL_POST_CONVOLUTION_RED_SCALE_EXT
}; // enum class pixel_transfer_parameter

} // namespace gl::group
