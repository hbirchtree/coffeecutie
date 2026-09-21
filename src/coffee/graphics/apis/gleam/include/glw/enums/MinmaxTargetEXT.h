#pragma once

#include "common.h"

namespace gl::group {

// MinmaxTargetEXT
enum class minmax_target_ext : u32
{
    minmax     = 0x802E, // GL_MINMAX
    minmax_ext = 0x802E, // GL_MINMAX_EXT
}; // enum class minmax_target_ext

} // namespace gl::group
