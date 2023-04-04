#pragma once

#include "common.h"

namespace gl::group {

// FogPointerTypeIBM
enum class fog_pointer_type_ibm : u32
{
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
}; // enum class fog_pointer_type_ibm

} // namespace gl::group
