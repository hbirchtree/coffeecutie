#pragma once

#include "common.h"

namespace gl::group {

// CheckFramebufferStatusTarget
enum class check_framebuffer_status_target : ::libc_types::u32
{
#ifdef GL_DRAW_FRAMEBUFFER
    draw_framebuffer = GL_DRAW_FRAMEBUFFER,
#endif
#ifdef GL_FRAMEBUFFER
    framebuffer = GL_FRAMEBUFFER,
#endif
#ifdef GL_READ_FRAMEBUFFER
    read_framebuffer = GL_READ_FRAMEBUFFER,
#endif
}; // enum class check_framebuffer_status_target

} // namespace gl::group
