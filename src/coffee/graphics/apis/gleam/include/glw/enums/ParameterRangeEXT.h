#pragma once

#include "common.h"

namespace gl::group {

// ParameterRangeEXT
enum class parameter_range_ext : u32
{
#ifdef GL_FULL_RANGE_EXT
    full_range_ext = GL_FULL_RANGE_EXT,
#endif
#ifdef GL_NORMALIZED_RANGE_EXT
    normalized_range_ext = GL_NORMALIZED_RANGE_EXT,
#endif
}; // enum class parameter_range_ext

} // namespace gl::group
