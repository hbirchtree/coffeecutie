#pragma once

#include "common.h"

namespace gl::groups {

// PathColor
enum class path_color : ::libc_types::u32 {
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

} // namespace gl::groups
