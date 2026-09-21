#pragma once

#include "common.h"

namespace gl::group {

// FogPointerTypeEXT
enum class fog_pointer_type_ext : u32
{
    double_ = 0x140A, // GL_DOUBLE
    float_  = 0x1406, // GL_FLOAT
}; // enum class fog_pointer_type_ext

} // namespace gl::group
