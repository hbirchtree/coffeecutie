#pragma once

#include "common.h"

namespace gl::group {

// FramebufferTarget
enum class framebuffer_target : u32
{
#ifdef GL_DRAW_FRAMEBUFFER
    draw_framebuffer = GL_DRAW_FRAMEBUFFER,
#endif
#ifdef GL_FRAMEBUFFER
    framebuffer = GL_FRAMEBUFFER,
#endif
#ifdef GL_FRAMEBUFFER_OES
    framebuffer_oes = GL_FRAMEBUFFER_OES,
#endif
#ifdef GL_READ_FRAMEBUFFER
    read_framebuffer = GL_READ_FRAMEBUFFER,
#endif
}; // enum class framebuffer_target

} // namespace gl::group
