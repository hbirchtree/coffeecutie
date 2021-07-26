#pragma once

#include "common.h"

namespace gl::group {

// ListNameType
enum class list_name_type : ::libc_types::u32
{
#ifdef GL_2_BYTES
    2_bytes = GL_2_BYTES,
#endif
#ifdef GL_3_BYTES
    3_bytes = GL_3_BYTES,
#endif
#ifdef GL_4_BYTES
    4_bytes = GL_4_BYTES,
#endif
#ifdef GL_BYTE
    byte_ = GL_BYTE,
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
}; // enum class list_name_type

} // namespace gl::group
