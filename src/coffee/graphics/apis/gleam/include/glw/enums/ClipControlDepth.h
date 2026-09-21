#pragma once

#include "common.h"

namespace gl::group {

// ClipControlDepth
enum class clip_control_depth : u32
{
    negative_one_to_one = 0x935E, // GL_NEGATIVE_ONE_TO_ONE
    zero_to_one         = 0x935F, // GL_ZERO_TO_ONE
}; // enum class clip_control_depth

} // namespace gl::group
