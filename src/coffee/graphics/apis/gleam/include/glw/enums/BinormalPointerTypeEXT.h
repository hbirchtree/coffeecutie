#pragma once

#include "common.h"

namespace gl::group {

// BinormalPointerTypeEXT
enum class binormal_pointer_type_ext : ::libc_types::u32
{
#ifdef GL_BYTE
    byte_ = GL_BYTE,
#endif
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
#ifdef GL_DOUBLE_EXT
    double_ext = GL_DOUBLE_EXT,
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
}; // enum class binormal_pointer_type_ext

} // namespace gl::group
