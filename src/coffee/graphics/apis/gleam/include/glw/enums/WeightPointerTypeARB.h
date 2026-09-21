#pragma once

#include "common.h"

namespace gl::group {

// WeightPointerTypeARB
enum class weight_pointer_type_arb : u32
{
    byte_          = 0x1400, // GL_BYTE
    double_        = 0x140A, // GL_DOUBLE
    float_         = 0x1406, // GL_FLOAT
    int_           = 0x1404, // GL_INT
    short_         = 0x1402, // GL_SHORT
    unsigned_byte  = 0x1401, // GL_UNSIGNED_BYTE
    unsigned_int   = 0x1405, // GL_UNSIGNED_INT
    unsigned_short = 0x1403, // GL_UNSIGNED_SHORT
}; // enum class weight_pointer_type_arb

} // namespace gl::group
