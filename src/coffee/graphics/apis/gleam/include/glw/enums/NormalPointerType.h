#pragma once

#include "common.h"

namespace gl::group {

// NormalPointerType
enum class normal_pointer_type : u32
{
    byte_   = 0x1400, // GL_BYTE
    double_ = 0x140A, // GL_DOUBLE
    float_  = 0x1406, // GL_FLOAT
    int_    = 0x1404, // GL_INT
    short_  = 0x1402, // GL_SHORT
}; // enum class normal_pointer_type

} // namespace gl::group
