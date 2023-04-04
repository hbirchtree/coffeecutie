#pragma once

#include "common.h"

namespace gl::group {

// ColorTableTargetSGI
enum class color_table_target_sgi : u32
{
#ifdef GL_COLOR_TABLE
    color_table = GL_COLOR_TABLE,
#endif
#ifdef GL_COLOR_TABLE_SGI
    color_table_sgi = GL_COLOR_TABLE_SGI,
#endif
#ifdef GL_POST_COLOR_MATRIX_COLOR_TABLE
    post_color_matrix_color_table = GL_POST_COLOR_MATRIX_COLOR_TABLE,
#endif
#ifdef GL_POST_COLOR_MATRIX_COLOR_TABLE_SGI
    post_color_matrix_color_table_sgi = GL_POST_COLOR_MATRIX_COLOR_TABLE_SGI,
#endif
#ifdef GL_POST_CONVOLUTION_COLOR_TABLE
    post_convolution_color_table = GL_POST_CONVOLUTION_COLOR_TABLE,
#endif
#ifdef GL_POST_CONVOLUTION_COLOR_TABLE_SGI
    post_convolution_color_table_sgi = GL_POST_CONVOLUTION_COLOR_TABLE_SGI,
#endif
#ifdef GL_PROXY_COLOR_TABLE
    proxy_color_table = GL_PROXY_COLOR_TABLE,
#endif
#ifdef GL_PROXY_COLOR_TABLE_SGI
    proxy_color_table_sgi = GL_PROXY_COLOR_TABLE_SGI,
#endif
#ifdef GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE
    proxy_post_color_matrix_color_table =
        GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE,
#endif
#ifdef GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE_SGI
    proxy_post_color_matrix_color_table_sgi =
        GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE_SGI,
#endif
#ifdef GL_PROXY_POST_CONVOLUTION_COLOR_TABLE
    proxy_post_convolution_color_table = GL_PROXY_POST_CONVOLUTION_COLOR_TABLE,
#endif
#ifdef GL_PROXY_POST_CONVOLUTION_COLOR_TABLE_SGI
    proxy_post_convolution_color_table_sgi =
        GL_PROXY_POST_CONVOLUTION_COLOR_TABLE_SGI,
#endif
#ifdef GL_PROXY_TEXTURE_COLOR_TABLE_SGI
    proxy_texture_color_table_sgi = GL_PROXY_TEXTURE_COLOR_TABLE_SGI,
#endif
#ifdef GL_TEXTURE_COLOR_TABLE_SGI
    texture_color_table_sgi = GL_TEXTURE_COLOR_TABLE_SGI,
#endif
}; // enum class color_table_target_sgi

} // namespace gl::group
