#pragma once

#include "common.h"

namespace gl::group {

// SeparableTargetEXT
enum class separable_target_ext : u32
{
    separable_2d     = 0x8012, // GL_SEPARABLE_2D
    separable_2d_ext = 0x8012, // GL_SEPARABLE_2D_EXT
}; // enum class separable_target_ext

} // namespace gl::group
