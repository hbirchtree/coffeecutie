#pragma once

#include "common.h"

namespace gl::group {

// ColorTableTargetSGI
enum class color_table_target_sgi : u32
{
    color_table                   = 0x80D0, // GL_COLOR_TABLE
    post_color_matrix_color_table = 0x80D2, // GL_POST_COLOR_MATRIX_COLOR_TABLE
    post_convolution_color_table  = 0x80D1, // GL_POST_CONVOLUTION_COLOR_TABLE
    proxy_color_table             = 0x80D3, // GL_PROXY_COLOR_TABLE
    proxy_post_color_matrix_color_table =
        0x80D5, // GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE
    proxy_post_convolution_color_table =
        0x80D4,               // GL_PROXY_POST_CONVOLUTION_COLOR_TABLE
    color_table_sgi = 0x80D0, // GL_COLOR_TABLE_SGI
    post_color_matrix_color_table_sgi =
        0x80D2, // GL_POST_COLOR_MATRIX_COLOR_TABLE_SGI
    post_convolution_color_table_sgi =
        0x80D1,                     // GL_POST_CONVOLUTION_COLOR_TABLE_SGI
    proxy_color_table_sgi = 0x80D3, // GL_PROXY_COLOR_TABLE_SGI
    proxy_post_color_matrix_color_table_sgi =
        0x80D5, // GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE_SGI
    proxy_post_convolution_color_table_sgi =
        0x80D4, // GL_PROXY_POST_CONVOLUTION_COLOR_TABLE_SGI
    proxy_texture_color_table_sgi = 0x80BD, // GL_PROXY_TEXTURE_COLOR_TABLE_SGI
    texture_color_table_sgi       = 0x80BC, // GL_TEXTURE_COLOR_TABLE_SGI
}; // enum class color_table_target_sgi

} // namespace gl::group
