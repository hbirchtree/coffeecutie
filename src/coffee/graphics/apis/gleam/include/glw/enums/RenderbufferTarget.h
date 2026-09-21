#pragma once

#include "common.h"

namespace gl::group {

// RenderbufferTarget
enum class renderbuffer_target : u32
{
    renderbuffer     = 0x8D41, // GL_RENDERBUFFER
    renderbuffer_oes = 0x8D41, // GL_RENDERBUFFER_OES
}; // enum class renderbuffer_target

} // namespace gl::group
