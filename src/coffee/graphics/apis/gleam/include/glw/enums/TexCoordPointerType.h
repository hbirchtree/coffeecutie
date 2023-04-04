#pragma once

#include "common.h"

namespace gl::group {

// TexCoordPointerType
enum class tex_coord_pointer_type : u32
{
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
#ifdef GL_INT
    int_ = GL_INT,
#endif
#ifdef GL_SHORT
    short_ = GL_SHORT,
#endif
}; // enum class tex_coord_pointer_type

} // namespace gl::group
