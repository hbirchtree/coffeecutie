#pragma once

#include "common.h"

namespace gl::group {

// TriangleFace
enum class triangle_face : u32
{
    back           = 0x0405, // GL_BACK
    front          = 0x0404, // GL_FRONT
    front_and_back = 0x0408, // GL_FRONT_AND_BACK
}; // enum class triangle_face

} // namespace gl::group
