#pragma once

#include "common.h"

namespace gl::group {

// PathFontStyle
enum class path_font_style : u32
{
#ifdef GL_BOLD_BIT_NV
    bold_bit_nv = GL_BOLD_BIT_NV,
#endif
#ifdef GL_ITALIC_BIT_NV
    italic_bit_nv = GL_ITALIC_BIT_NV,
#endif
#ifdef GL_NONE
    none = GL_NONE,
#endif
}; // enum class path_font_style
C_FLAGS(path_font_style, ::libc_types::u32);

} // namespace gl::group
