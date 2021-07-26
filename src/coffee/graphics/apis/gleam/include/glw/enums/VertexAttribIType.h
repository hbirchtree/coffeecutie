#pragma once

#include "common.h"

namespace gl::group {

// VertexAttribIType
enum class vertex_attrib_int : ::libc_types::u32
{
#ifdef GL_BYTE
    byte_ = GL_BYTE,
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
}; // enum class vertex_attrib_int

} // namespace gl::group
