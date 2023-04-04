#pragma once

#include "common.h"

namespace gl::group {

// TriangleFace
enum class triangle_face : u32
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
}; // enum class triangle_face

} // namespace gl::group
