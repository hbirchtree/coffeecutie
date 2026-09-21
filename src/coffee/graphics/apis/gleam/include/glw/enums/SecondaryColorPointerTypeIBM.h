#pragma once

#include "common.h"

namespace gl::group {

// SecondaryColorPointerTypeIBM
enum class secondary_color_pointer_type_ibm : u32
{
    double_ = 0x140A, // GL_DOUBLE
    float_  = 0x1406, // GL_FLOAT
    int_    = 0x1404, // GL_INT
    short_  = 0x1402, // GL_SHORT
}; // enum class secondary_color_pointer_type_ibm

} // namespace gl::group
