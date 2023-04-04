#pragma once

#include "common.h"

namespace gl::group {

// TextureCoordName
enum class texture_coord_name : u32
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
#ifdef GL_TEXTURE_GEN_STR_OES
    texture_gen_str_oes = GL_TEXTURE_GEN_STR_OES,
#endif
}; // enum class texture_coord_name

} // namespace gl::group
