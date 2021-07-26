#pragma once

#include "common.h"

namespace gl::group {

// PrecisionType
enum class precision_type : ::libc_types::u32
{
#ifdef GL_HIGH_FLOAT
    high_float = GL_HIGH_FLOAT,
#endif
#ifdef GL_HIGH_INT
    high_int = GL_HIGH_INT,
#endif
#ifdef GL_LOW_FLOAT
    low_float = GL_LOW_FLOAT,
#endif
#ifdef GL_LOW_INT
    low_int = GL_LOW_INT,
#endif
#ifdef GL_MEDIUM_FLOAT
    medium_float = GL_MEDIUM_FLOAT,
#endif
#ifdef GL_MEDIUM_INT
    medium_int = GL_MEDIUM_INT,
#endif
}; // enum class precision_type

} // namespace gl::group
