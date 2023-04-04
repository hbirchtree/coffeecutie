#pragma once

#include "common.h"

namespace gl::group {

// RenderbufferTarget
enum class renderbuffer_target : u32
{
#ifdef GL_RENDERBUFFER
    renderbuffer = GL_RENDERBUFFER,
#endif
#ifdef GL_RENDERBUFFER_OES
    renderbuffer_oes = GL_RENDERBUFFER_OES,
#endif
}; // enum class renderbuffer_target

} // namespace gl::group
