#pragma once

#include "common.h"

namespace gl::group {

// PathColorFormat
enum class path_color_format : u32
{
    alpha           = 0x1906, // GL_ALPHA
    intensity       = 0x8049, // GL_INTENSITY
    luminance       = 0x1909, // GL_LUMINANCE
    luminance_alpha = 0x190A, // GL_LUMINANCE_ALPHA
    none            = 0,      // GL_NONE
    rgb             = 0x1907, // GL_RGB
    rgba            = 0x1908, // GL_RGBA
}; // enum class path_color_format

} // namespace gl::group
