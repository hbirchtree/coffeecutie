#pragma once

#include "common.h"

namespace gl::group {

// CullFaceMode
enum class cull_face_mode : ::libc_types::u32
{
#ifdef GL_BACK
    back = GL_BACK,
#endif
#ifdef GL_FRONT
    front = GL_FRONT,
#endif
#ifdef GL_FRONT_AND_BACK
    front_and_back = GL_FRONT_AND_BACK,
#endif
}; // enum class cull_face_mode

} // namespace gl::group
