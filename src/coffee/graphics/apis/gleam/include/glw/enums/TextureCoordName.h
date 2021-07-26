#pragma once

#include "common.h"

namespace gl::group {

// TextureCoordName
enum class texture_coord_name : ::libc_types::u32
{
#ifdef GL_Q
    q = GL_Q,
#endif
#ifdef GL_R
    r = GL_R,
#endif
#ifdef GL_S
    s = GL_S,
#endif
#ifdef GL_T
    t = GL_T,
#endif
}; // enum class texture_coord_name

} // namespace gl::group
