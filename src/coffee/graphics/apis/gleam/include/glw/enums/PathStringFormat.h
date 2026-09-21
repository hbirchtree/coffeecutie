#pragma once

#include "common.h"

namespace gl::group {

// PathStringFormat
enum class path_string_format : u32
{
    path_format_ps_nv  = 0x9071, // GL_PATH_FORMAT_PS_NV
    path_format_svg_nv = 0x9070, // GL_PATH_FORMAT_SVG_NV
}; // enum class path_string_format

} // namespace gl::group
