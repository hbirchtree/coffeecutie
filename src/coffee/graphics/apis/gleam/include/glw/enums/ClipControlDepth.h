#pragma once

#include "common.h"

namespace gl::group {

// ClipControlDepth
enum class clip_control_depth : u32
{
#ifdef GL_NEGATIVE_ONE_TO_ONE
    negative_one_to_one = GL_NEGATIVE_ONE_TO_ONE,
#endif
#ifdef GL_ZERO_TO_ONE
    zero_to_one = GL_ZERO_TO_ONE,
#endif
}; // enum class clip_control_depth

} // namespace gl::group
