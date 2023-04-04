#pragma once

#include "common.h"

namespace gl::group {

// FogPointerTypeEXT
enum class fog_pointer_type_ext : u32
{
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
}; // enum class fog_pointer_type_ext

} // namespace gl::group
