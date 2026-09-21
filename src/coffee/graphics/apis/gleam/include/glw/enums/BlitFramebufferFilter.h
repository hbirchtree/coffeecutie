#pragma once

#include "common.h"

namespace gl::group {

// BlitFramebufferFilter
enum class blit_framebuffer_filter : u32
{
    linear  = 0x2601, // GL_LINEAR
    nearest = 0x2600, // GL_NEAREST
}; // enum class blit_framebuffer_filter

} // namespace gl::group
