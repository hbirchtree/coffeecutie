#pragma once

#include "common.h"

namespace gl::group {

// ColorTableTarget
enum class color_table_target : u32
{
#ifdef GL_COLOR_TABLE
    color_table = GL_COLOR_TABLE,
#endif
#ifdef GL_POST_COLOR_MATRIX_COLOR_TABLE
    post_color_matrix_color_table = GL_POST_COLOR_MATRIX_COLOR_TABLE,
#endif
#ifdef GL_POST_CONVOLUTION_COLOR_TABLE
    post_convolution_color_table = GL_POST_CONVOLUTION_COLOR_TABLE,
#endif
#ifdef GL_PROXY_COLOR_TABLE
    proxy_color_table = GL_PROXY_COLOR_TABLE,
#endif
#ifdef GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE
    proxy_post_color_matrix_color_table =
        GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE,
#endif
#ifdef GL_PROXY_POST_CONVOLUTION_COLOR_TABLE
    proxy_post_convolution_color_table = GL_PROXY_POST_CONVOLUTION_COLOR_TABLE,
#endif
}; // enum class color_table_target

} // namespace gl::group
