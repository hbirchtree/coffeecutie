#pragma once

#include "common.h"

namespace gl::group {

// VertexAttribPointerType
enum class vertex_attrib_pointer_type : u32
{
    byte_                        = 0x1400, // GL_BYTE
    double_                      = 0x140A, // GL_DOUBLE
    fixed                        = 0x140C, // GL_FIXED
    float_                       = 0x1406, // GL_FLOAT
    half_float                   = 0x140B, // GL_HALF_FLOAT
    int_                         = 0x1404, // GL_INT
    int_2_10_10_10_rev           = 0x8D9F, // GL_INT_2_10_10_10_REV
    short_                       = 0x1402, // GL_SHORT
    unsigned_byte                = 0x1401, // GL_UNSIGNED_BYTE
    unsigned_int                 = 0x1405, // GL_UNSIGNED_INT
    unsigned_int_10f_11f_11f_rev = 0x8C3B, // GL_UNSIGNED_INT_10F_11F_11F_REV
    unsigned_int_2_10_10_10_rev  = 0x8368, // GL_UNSIGNED_INT_2_10_10_10_REV
    unsigned_short               = 0x1403, // GL_UNSIGNED_SHORT
    int64_arb                    = 0x140E, // GL_INT64_ARB
    int64_nv                     = 0x140E, // GL_INT64_NV
    unsigned_int64_arb           = 0x140F, // GL_UNSIGNED_INT64_ARB
    unsigned_int64_nv            = 0x140F, // GL_UNSIGNED_INT64_NV
    unsigned_int_2_10_10_10_rev_ext =
        0x8368, // GL_UNSIGNED_INT_2_10_10_10_REV_EXT
}; // enum class vertex_attrib_pointer_type

} // namespace gl::group
