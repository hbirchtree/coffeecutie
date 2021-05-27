#pragma once

#include "common.h"

namespace gl::groups {

// MatrixMode
enum class matrix_mode : ::libc_types::u32 {
#ifdef GL_MODELVIEW
    modelview = GL_MODELVIEW,
#endif
#ifdef GL_MODELVIEW0_EXT
    modelview0_ext = GL_MODELVIEW0_EXT,
#endif
#ifdef GL_PROJECTION
    projection = GL_PROJECTION,
#endif
#ifdef GL_TEXTURE
    texture = GL_TEXTURE,
#endif
}; // enum class matrix_mode

} // namespace gl::groups
