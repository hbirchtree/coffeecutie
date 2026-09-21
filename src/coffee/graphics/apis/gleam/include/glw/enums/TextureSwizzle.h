#pragma once

#include "common.h"

namespace gl::group {

// TextureSwizzle
enum class texture_swizzle : u32
{
    alpha = 0x1906, // GL_ALPHA
    blue  = 0x1905, // GL_BLUE
    green = 0x1904, // GL_GREEN
    one   = 1,      // GL_ONE
    red   = 0x1903, // GL_RED
    zero  = 0,      // GL_ZERO
}; // enum class texture_swizzle

} // namespace gl::group
