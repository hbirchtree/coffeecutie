#pragma once

#include "common.h"

namespace gl::group {

// MapParameterNV
enum class map_parameter_nv : ::libc_types::u32
{
#ifdef GL_MAP_TESSELLATION_NV
    map_tessellation_nv = GL_MAP_TESSELLATION_NV,
#endif
}; // enum class map_parameter_nv

} // namespace gl::group
