#pragma once

#include "common.h"

namespace gl::groups {

// Buffer
enum class buffer : ::libc_types::u32 {
#ifdef GL_COLOR
    color = GL_COLOR,
#endif
#ifdef GL_DEPTH
    depth = GL_DEPTH,
#endif
#ifdef GL_STENCIL
    stencil = GL_STENCIL,
#endif
}; // enum class buffer

} // namespace gl::groups
