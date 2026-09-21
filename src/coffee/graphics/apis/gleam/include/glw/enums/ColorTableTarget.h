#pragma once

#include "common.h"

namespace gl::group {

// ColorTableTarget
enum class color_table_target : u32
{
    color_table                   = 0x80D0, // GL_COLOR_TABLE
    post_color_matrix_color_table = 0x80D2, // GL_POST_COLOR_MATRIX_COLOR_TABLE
    post_convolution_color_table  = 0x80D1, // GL_POST_CONVOLUTION_COLOR_TABLE
    proxy_color_table             = 0x80D3, // GL_PROXY_COLOR_TABLE
    proxy_post_color_matrix_color_table =
        0x80D5, // GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE
    proxy_post_convolution_color_table =
        0x80D4, // GL_PROXY_POST_CONVOLUTION_COLOR_TABLE
}; // enum class color_table_target

} // namespace gl::group
