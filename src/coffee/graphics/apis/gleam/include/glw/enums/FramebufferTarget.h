#pragma once

#include "common.h"

namespace gl::group {

// FramebufferTarget
enum class framebuffer_target : u32
{
    draw_framebuffer = 0x8CA9, // GL_DRAW_FRAMEBUFFER
    framebuffer      = 0x8D40, // GL_FRAMEBUFFER
    read_framebuffer = 0x8CA8, // GL_READ_FRAMEBUFFER
    framebuffer_oes  = 0x8D40, // GL_FRAMEBUFFER_OES
}; // enum class framebuffer_target

} // namespace gl::group
