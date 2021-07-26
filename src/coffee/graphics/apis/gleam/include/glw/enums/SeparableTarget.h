#pragma once

#include "common.h"

namespace gl::group {

// SeparableTarget
enum class separable_target : ::libc_types::u32
{
#ifdef GL_SEPARABLE_2D
    separable_2d = GL_SEPARABLE_2D,
#endif
}; // enum class separable_target

} // namespace gl::group
