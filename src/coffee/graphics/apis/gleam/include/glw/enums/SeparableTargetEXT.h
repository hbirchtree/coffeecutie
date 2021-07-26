#pragma once

#include "common.h"

namespace gl::group {

// SeparableTargetEXT
enum class separable_target_ext : ::libc_types::u32
{
#ifdef GL_SEPARABLE_2D
    separable_2d = GL_SEPARABLE_2D,
#endif
#ifdef GL_SEPARABLE_2D_EXT
    separable_2d_ext = GL_SEPARABLE_2D_EXT,
#endif
}; // enum class separable_target_ext

} // namespace gl::group
