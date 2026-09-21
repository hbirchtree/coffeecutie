#pragma once

#include "common.h"

namespace gl::group {

// FogCoordinatePointerType
enum class fog_coordinate_pointer_type : u32
{
    double_ = 0x140A, // GL_DOUBLE
    float_  = 0x1406, // GL_FLOAT
}; // enum class fog_coordinate_pointer_type

} // namespace gl::group
