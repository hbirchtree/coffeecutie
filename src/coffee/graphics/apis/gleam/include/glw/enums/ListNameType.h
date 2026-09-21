#pragma once

#include "common.h"

namespace gl::group {

// ListNameType
enum class list_name_type : u32
{
    n2_bytes       = 0x1407, // GL_2_BYTES
    n3_bytes       = 0x1408, // GL_3_BYTES
    n4_bytes       = 0x1409, // GL_4_BYTES
    byte_          = 0x1400, // GL_BYTE
    float_         = 0x1406, // GL_FLOAT
    int_           = 0x1404, // GL_INT
    short_         = 0x1402, // GL_SHORT
    unsigned_byte  = 0x1401, // GL_UNSIGNED_BYTE
    unsigned_int   = 0x1405, // GL_UNSIGNED_INT
    unsigned_short = 0x1403, // GL_UNSIGNED_SHORT
}; // enum class list_name_type

} // namespace gl::group
