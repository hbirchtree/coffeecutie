#pragma once

#include "common.h"

namespace gl::group {

// BinormalPointerTypeEXT
enum class binormal_pointer_type_ext : u32
{
    byte_      = 0x1400, // GL_BYTE
    double_    = 0x140A, // GL_DOUBLE
    float_     = 0x1406, // GL_FLOAT
    int_       = 0x1404, // GL_INT
    short_     = 0x1402, // GL_SHORT
    double_ext = 0x140A, // GL_DOUBLE_EXT
}; // enum class binormal_pointer_type_ext

} // namespace gl::group
