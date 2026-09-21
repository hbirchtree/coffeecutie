#pragma once

#include "common.h"

namespace gl::group {

// VertexAttribIType
enum class vertex_attrib_int : u32
{
    byte_          = 0x1400, // GL_BYTE
    int_           = 0x1404, // GL_INT
    short_         = 0x1402, // GL_SHORT
    unsigned_byte  = 0x1401, // GL_UNSIGNED_BYTE
    unsigned_int   = 0x1405, // GL_UNSIGNED_INT
    unsigned_short = 0x1403, // GL_UNSIGNED_SHORT
}; // enum class vertex_attrib_int

} // namespace gl::group
