#pragma once

#include "common.h"

namespace gl::groups {

// MinmaxTarget
enum class minmax_target : ::libc_types::u32 {
#ifdef GL_MINMAX
    minmax = GL_MINMAX,
#endif
}; // enum class minmax_target

} // namespace gl::groups
