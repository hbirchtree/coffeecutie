#pragma once

#include "common.h"

namespace gl::group {

// FogCoordinatePointerType
enum class fog_coordinate_pointer_type : ::libc_types::u32
{
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
}; // enum class fog_coordinate_pointer_type

} // namespace gl::group
