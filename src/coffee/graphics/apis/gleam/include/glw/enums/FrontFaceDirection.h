#pragma once

#include "common.h"

namespace gl::group {

// FrontFaceDirection
enum class front_face_direction : u32
{
#ifdef GL_CCW
    ccw = GL_CCW,
#endif
#ifdef GL_CW
    cw = GL_CW,
#endif
}; // enum class front_face_direction

} // namespace gl::group
