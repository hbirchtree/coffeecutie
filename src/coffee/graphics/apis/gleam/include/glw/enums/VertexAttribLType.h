#pragma once

#include "common.h"

namespace gl::group {

// VertexAttribLType
enum class vertex_attrib_long : u32
{
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
}; // enum class vertex_attrib_long

} // namespace gl::group
