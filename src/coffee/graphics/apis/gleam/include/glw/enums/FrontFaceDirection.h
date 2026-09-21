#pragma once

#include "common.h"

namespace gl::group {

// FrontFaceDirection
enum class front_face_direction : u32
{
    ccw = 0x0901, // GL_CCW
    cw  = 0x0900, // GL_CW
}; // enum class front_face_direction

} // namespace gl::group
