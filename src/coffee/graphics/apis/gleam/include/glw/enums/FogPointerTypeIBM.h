#pragma once

#include "common.h"

namespace gl::group {

// FogPointerTypeIBM
enum class fog_pointer_type_ibm : u32
{
    double_ = 0x140A, // GL_DOUBLE
    float_  = 0x1406, // GL_FLOAT
}; // enum class fog_pointer_type_ibm

} // namespace gl::group
