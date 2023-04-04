#pragma once

#include "common.h"

namespace gl::group {

// BlitFramebufferFilter
enum class blit_framebuffer_filter : u32
{
#ifdef GL_LINEAR
    linear = GL_LINEAR,
#endif
#ifdef GL_NEAREST
    nearest = GL_NEAREST,
#endif
}; // enum class blit_framebuffer_filter

} // namespace gl::group
