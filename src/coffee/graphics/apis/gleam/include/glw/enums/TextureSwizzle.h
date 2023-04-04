#pragma once

#include "common.h"

namespace gl::group {

// TextureSwizzle
enum class texture_swizzle : u32
{
#ifdef GL_ALPHA
    alpha = GL_ALPHA,
#endif
#ifdef GL_BLUE
    blue = GL_BLUE,
#endif
#ifdef GL_GREEN
    green = GL_GREEN,
#endif
#ifdef GL_ONE
    one = GL_ONE,
#endif
#ifdef GL_RED
    red = GL_RED,
#endif
#ifdef GL_ZERO
    zero = GL_ZERO,
#endif
}; // enum class texture_swizzle

} // namespace gl::group
