#pragma once

#include "common.h"

namespace gl::group {

// PrecisionType
enum class precision_type : u32
{
    high_float   = 0x8DF2, // GL_HIGH_FLOAT
    high_int     = 0x8DF5, // GL_HIGH_INT
    low_float    = 0x8DF0, // GL_LOW_FLOAT
    low_int      = 0x8DF3, // GL_LOW_INT
    medium_float = 0x8DF1, // GL_MEDIUM_FLOAT
    medium_int   = 0x8DF4, // GL_MEDIUM_INT
}; // enum class precision_type

} // namespace gl::group
