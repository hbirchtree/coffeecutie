#pragma once

#include "common.h"

namespace gl::group {

// VertexAttribPointerType
enum class vertex_attrib_pointer_type : ::libc_types::u32
{
#ifdef GL_BYTE
    byte_ = GL_BYTE,
#endif
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
#ifdef GL_FIXED
    fixed = GL_FIXED,
#endif
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
#ifdef GL_HALF_FLOAT
    half_float = GL_HALF_FLOAT,
#endif
#ifdef GL_INT
    int_ = GL_INT,
#endif
#ifdef GL_INT64_ARB
    int64_arb = GL_INT64_ARB,
#endif
#ifdef GL_INT64_NV
    int64_nv = GL_INT64_NV,
#endif
#ifdef GL_INT_2_10_10_10_REV
    int_2_10_10_10_rev = GL_INT_2_10_10_10_REV,
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
#ifdef GL_UNSIGNED_INT64_ARB
    unsigned_int64_arb = GL_UNSIGNED_INT64_ARB,
#endif
#ifdef GL_UNSIGNED_INT64_NV
    unsigned_int64_nv = GL_UNSIGNED_INT64_NV,
#endif
#ifdef GL_UNSIGNED_INT_10F_11F_11F_REV
    unsigned_int_10f_11f_11f_rev = GL_UNSIGNED_INT_10F_11F_11F_REV,
#endif
#ifdef GL_UNSIGNED_INT_2_10_10_10_REV
    unsigned_int_2_10_10_10_rev = GL_UNSIGNED_INT_2_10_10_10_REV,
#endif
#ifdef GL_UNSIGNED_INT_2_10_10_10_REV_EXT
    unsigned_int_2_10_10_10_rev_ext = GL_UNSIGNED_INT_2_10_10_10_REV_EXT,
#endif
#ifdef GL_UNSIGNED_SHORT
    unsigned_short = GL_UNSIGNED_SHORT,
#endif
}; // enum class vertex_attrib_pointer_type

} // namespace gl::group
