#pragma once

#include "common.h"

namespace gl::group {

// PathFontStyle
enum class path_font_style : u32
{
    none          = 0,    // GL_NONE
    bold_bit_nv   = 0x01, // GL_BOLD_BIT_NV
    italic_bit_nv = 0x02, // GL_ITALIC_BIT_NV
}; // enum class path_font_style
C_FLAGS(path_font_style, u32);

} // namespace gl::group
