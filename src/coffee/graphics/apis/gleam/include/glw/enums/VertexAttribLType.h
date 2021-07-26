#pragma once

#include "common.h"

namespace gl::group {

// VertexAttribLType
enum class vertex_attrib_long : ::libc_types::u32
{
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
}; // enum class vertex_attrib_long

} // namespace gl::group
