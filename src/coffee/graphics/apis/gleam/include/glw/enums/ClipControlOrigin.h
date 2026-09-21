#pragma once

#include "common.h"

namespace gl::group {

// ClipControlOrigin
enum class clip_control_origin : u32
{
    lower_left = 0x8CA1, // GL_LOWER_LEFT
    upper_left = 0x8CA2, // GL_UPPER_LEFT
}; // enum class clip_control_origin

} // namespace gl::group
