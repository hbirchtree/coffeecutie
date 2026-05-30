#pragma once

#include "common.h"

namespace gl::group {

// ColorPointerType
enum class color_pointer_type : u32
{
#ifdef GL_BYTE
    byte_ = GL_BYTE,
#endif
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
#ifdef GL_UNSIGNED_BYTE
    unsigned_byte = GL_UNSIGNED_BYTE,
#endif
#ifdef GL_UNSIGNED_INT
    unsigned_int = GL_UNSIGNED_INT,
#endif
#ifdef GL_UNSIGNED_SHORT
    unsigned_short = GL_UNSIGNED_SHORT,
#endif
}; // enum class color_pointer_type

} // namespace gl::group
