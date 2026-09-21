#pragma once

#include "common.h"

namespace gl::group {

// PathFontTarget
enum class path_font_target : u32
{
    file_name_nv          = 0x9074, // GL_FILE_NAME_NV
    standard_font_name_nv = 0x9072, // GL_STANDARD_FONT_NAME_NV
    system_font_name_nv   = 0x9073, // GL_SYSTEM_FONT_NAME_NV
}; // enum class path_font_target

} // namespace gl::group
