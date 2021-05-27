#pragma once

#include "common.h"

namespace gl::groups {

// MapTypeNV
enum class map_type_nv : ::libc_types::u32 {
#ifdef GL_DOUBLE
    double_ = GL_DOUBLE,
#endif
#ifdef GL_FLOAT
    float_ = GL_FLOAT,
#endif
}; // enum class map_type_nv

} // namespace gl::groups
