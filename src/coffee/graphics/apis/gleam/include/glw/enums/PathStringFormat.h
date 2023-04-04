#pragma once

#include "common.h"

namespace gl::group {

// PathStringFormat
enum class path_string_format : u32
{
#ifdef GL_PATH_FORMAT_PS_NV
    path_format_ps_nv = GL_PATH_FORMAT_PS_NV,
#endif
#ifdef GL_PATH_FORMAT_SVG_NV
    path_format_svg_nv = GL_PATH_FORMAT_SVG_NV,
#endif
}; // enum class path_string_format

} // namespace gl::group
