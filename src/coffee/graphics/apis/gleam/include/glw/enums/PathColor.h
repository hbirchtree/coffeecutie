#pragma once

#include "common.h"

namespace gl::group {

// PathColor
enum class path_color : u32
{
    primary_color      = 0x8577, // GL_PRIMARY_COLOR
    primary_color_nv   = 0x852C, // GL_PRIMARY_COLOR_NV
    secondary_color_nv = 0x852D, // GL_SECONDARY_COLOR_NV
}; // enum class path_color

} // namespace gl::group
