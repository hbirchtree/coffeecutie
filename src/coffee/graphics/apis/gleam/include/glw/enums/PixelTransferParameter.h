#pragma once

#include "common.h"

namespace gl::group {

// PixelTransferParameter
enum class pixel_transfer_parameter : ::libc_types::u32
{
#ifdef GL_ALPHA_BIAS
    alpha_bias = GL_ALPHA_BIAS,
#endif
#ifdef GL_ALPHA_SCALE
    alpha_scale = GL_ALPHA_SCALE,
#endif
#ifdef GL_BLUE_BIAS
    blue_bias = GL_BLUE_BIAS,
#endif
#ifdef GL_BLUE_SCALE
    blue_scale = GL_BLUE_SCALE,
#endif
#ifdef GL_DEPTH_BIAS
    depth_bias = GL_DEPTH_BIAS,
#endif
#ifdef GL_DEPTH_SCALE
    depth_scale = GL_DEPTH_SCALE,
#endif
#ifdef GL_GREEN_BIAS
    green_bias = GL_GREEN_BIAS,
#endif
#ifdef GL_GREEN_SCALE
    green_scale = GL_GREEN_SCALE,
#endif
#ifdef GL_INDEX_OFFSET
    index_offset = GL_INDEX_OFFSET,
#endif
#ifdef GL_INDEX_SHIFT
    index_shift = GL_INDEX_SHIFT,
#endif
#ifdef GL_MAP_COLOR
    map_color = GL_MAP_COLOR,
#endif
#ifdef GL_MAP_STENCIL
    map_stencil = GL_MAP_STENCIL,
#endif
#ifdef GL_POST_COLOR_MATRIX_ALPHA_BIAS
    post_color_matrix_alpha_bias = GL_POST_COLOR_MATRIX_ALPHA_BIAS,
#endif
#ifdef GL_POST_COLOR_MATRIX_ALPHA_BIAS_SGI
    post_color_matrix_alpha_bias_sgi = GL_POST_COLOR_MATRIX_ALPHA_BIAS_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_ALPHA_SCALE
    post_color_matrix_alpha_scale = GL_POST_COLOR_MATRIX_ALPHA_SCALE,
#endif
#ifdef GL_POST_COLOR_MATRIX_ALPHA_SCALE_SGI
    post_color_matrix_alpha_scale_sgi = GL_POST_COLOR_MATRIX_ALPHA_SCALE_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_BLUE_BIAS
    post_color_matrix_blue_bias = GL_POST_COLOR_MATRIX_BLUE_BIAS,
#endif
#ifdef GL_POST_COLOR_MATRIX_BLUE_BIAS_SGI
    post_color_matrix_blue_bias_sgi = GL_POST_COLOR_MATRIX_BLUE_BIAS_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_BLUE_SCALE
    post_color_matrix_blue_scale = GL_POST_COLOR_MATRIX_BLUE_SCALE,
#endif
#ifdef GL_POST_COLOR_MATRIX_BLUE_SCALE_SGI
    post_color_matrix_blue_scale_sgi = GL_POST_COLOR_MATRIX_BLUE_SCALE_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_GREEN_BIAS
    post_color_matrix_green_bias = GL_POST_COLOR_MATRIX_GREEN_BIAS,
#endif
#ifdef GL_POST_COLOR_MATRIX_GREEN_BIAS_SGI
    post_color_matrix_green_bias_sgi = GL_POST_COLOR_MATRIX_GREEN_BIAS_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_GREEN_SCALE
    post_color_matrix_green_scale = GL_POST_COLOR_MATRIX_GREEN_SCALE,
#endif
#ifdef GL_POST_COLOR_MATRIX_GREEN_SCALE_SGI
    post_color_matrix_green_scale_sgi = GL_POST_COLOR_MATRIX_GREEN_SCALE_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_RED_BIAS
    post_color_matrix_red_bias = GL_POST_COLOR_MATRIX_RED_BIAS,
#endif
#ifdef GL_POST_COLOR_MATRIX_RED_BIAS_SGI
    post_color_matrix_red_bias_sgi = GL_POST_COLOR_MATRIX_RED_BIAS_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_RED_SCALE
    post_color_matrix_red_scale = GL_POST_COLOR_MATRIX_RED_SCALE,
#endif
#ifdef GL_POST_COLOR_MATRIX_RED_SCALE_SGI
    post_color_matrix_red_scale_sgi = GL_POST_COLOR_MATRIX_RED_SCALE_SGI,
#endif
#ifdef GL_POST_CONVOLUTION_ALPHA_BIAS
    post_convolution_alpha_bias = GL_POST_CONVOLUTION_ALPHA_BIAS,
#endif
#ifdef GL_POST_CONVOLUTION_ALPHA_BIAS_EXT
    post_convolution_alpha_bias_ext = GL_POST_CONVOLUTION_ALPHA_BIAS_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_ALPHA_SCALE
    post_convolution_alpha_scale = GL_POST_CONVOLUTION_ALPHA_SCALE,
#endif
#ifdef GL_POST_CONVOLUTION_ALPHA_SCALE_EXT
    post_convolution_alpha_scale_ext = GL_POST_CONVOLUTION_ALPHA_SCALE_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_BLUE_BIAS
    post_convolution_blue_bias = GL_POST_CONVOLUTION_BLUE_BIAS,
#endif
#ifdef GL_POST_CONVOLUTION_BLUE_BIAS_EXT
    post_convolution_blue_bias_ext = GL_POST_CONVOLUTION_BLUE_BIAS_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_BLUE_SCALE
    post_convolution_blue_scale = GL_POST_CONVOLUTION_BLUE_SCALE,
#endif
#ifdef GL_POST_CONVOLUTION_BLUE_SCALE_EXT
    post_convolution_blue_scale_ext = GL_POST_CONVOLUTION_BLUE_SCALE_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_GREEN_BIAS
    post_convolution_green_bias = GL_POST_CONVOLUTION_GREEN_BIAS,
#endif
#ifdef GL_POST_CONVOLUTION_GREEN_BIAS_EXT
    post_convolution_green_bias_ext = GL_POST_CONVOLUTION_GREEN_BIAS_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_GREEN_SCALE
    post_convolution_green_scale = GL_POST_CONVOLUTION_GREEN_SCALE,
#endif
#ifdef GL_POST_CONVOLUTION_GREEN_SCALE_EXT
    post_convolution_green_scale_ext = GL_POST_CONVOLUTION_GREEN_SCALE_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_RED_BIAS
    post_convolution_red_bias = GL_POST_CONVOLUTION_RED_BIAS,
#endif
#ifdef GL_POST_CONVOLUTION_RED_BIAS_EXT
    post_convolution_red_bias_ext = GL_POST_CONVOLUTION_RED_BIAS_EXT,
#endif
#ifdef GL_POST_CONVOLUTION_RED_SCALE
    post_convolution_red_scale = GL_POST_CONVOLUTION_RED_SCALE,
#endif
#ifdef GL_POST_CONVOLUTION_RED_SCALE_EXT
    post_convolution_red_scale_ext = GL_POST_CONVOLUTION_RED_SCALE_EXT,
#endif
#ifdef GL_RED_BIAS
    red_bias = GL_RED_BIAS,
#endif
#ifdef GL_RED_SCALE
    red_scale = GL_RED_SCALE,
#endif
}; // enum class pixel_transfer_parameter

} // namespace gl::group
