#pragma once

#include "common.h"

namespace gl::group {

// ParameterRangeEXT
enum class parameter_range_ext : u32
{
    full_range_ext       = 0x87E1, // GL_FULL_RANGE_EXT
    normalized_range_ext = 0x87E0, // GL_NORMALIZED_RANGE_EXT
}; // enum class parameter_range_ext

} // namespace gl::group
