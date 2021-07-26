#pragma once

#include "common.h"

namespace gl::group {

// PathFontTarget
enum class path_font_target : ::libc_types::u32
{
#ifdef GL_FILE_NAME_NV
    file_name_nv = GL_FILE_NAME_NV,
#endif
#ifdef GL_STANDARD_FONT_NAME_NV
    standard_font_name_nv = GL_STANDARD_FONT_NAME_NV,
#endif
#ifdef GL_SYSTEM_FONT_NAME_NV
    system_font_name_nv = GL_SYSTEM_FONT_NAME_NV,
#endif
}; // enum class path_font_target

} // namespace gl::group
