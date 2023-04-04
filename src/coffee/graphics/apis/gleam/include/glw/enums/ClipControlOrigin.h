#pragma once

#include "common.h"

namespace gl::group {

// ClipControlOrigin
enum class clip_control_origin : u32
{
#ifdef GL_LOWER_LEFT
    lower_left = GL_LOWER_LEFT,
#endif
#ifdef GL_UPPER_LEFT
    upper_left = GL_UPPER_LEFT,
#endif
}; // enum class clip_control_origin

} // namespace gl::group
