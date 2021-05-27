#pragma once

#include "common.h"

namespace gl::groups {

// MinmaxTargetEXT
enum class minmax_target_ext : ::libc_types::u32 {
#ifdef GL_MINMAX
    minmax = GL_MINMAX,
#endif
#ifdef GL_MINMAX_EXT
    minmax_ext = GL_MINMAX_EXT,
#endif
}; // enum class minmax_target_ext

} // namespace gl::groups
