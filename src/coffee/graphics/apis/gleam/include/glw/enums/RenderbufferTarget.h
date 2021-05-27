#pragma once

#include "common.h"

namespace gl::groups {

// RenderbufferTarget
enum class renderbuffer_target : ::libc_types::u32 {
#ifdef GL_RENDERBUFFER
    renderbuffer = GL_RENDERBUFFER,
#endif
#ifdef GL_RENDERBUFFER_OES
    renderbuffer_oes = GL_RENDERBUFFER_OES,
#endif
}; // enum class renderbuffer_target

} // namespace gl::groups
