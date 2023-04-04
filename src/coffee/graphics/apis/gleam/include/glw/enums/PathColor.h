#pragma once

#include "common.h"

namespace gl::group {

// PathColor
enum class path_color : u32
{
#ifdef GL_PRIMARY_COLOR
    primary_color = GL_PRIMARY_COLOR,
#endif
#ifdef GL_PRIMARY_COLOR_NV
    primary_color_nv = GL_PRIMARY_COLOR_NV,
#endif
#ifdef GL_SECONDARY_COLOR_NV
    secondary_color_nv = GL_SECONDARY_COLOR_NV,
#endif
}; // enum class path_color

} // namespace gl::group
