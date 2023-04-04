#pragma once

#include "common.h"

namespace gl::group {

// MinmaxTargetEXT
enum class minmax_target_ext : u32
{
#ifdef GL_MINMAX
    minmax = GL_MINMAX,
#endif
#ifdef GL_MINMAX_EXT
    minmax_ext = GL_MINMAX_EXT,
#endif
}; // enum class minmax_target_ext

} // namespace gl::group
