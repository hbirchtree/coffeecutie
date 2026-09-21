#pragma once

#include "common.h"

namespace gl::group {

// DrawElementsType
enum class draw_elements_type : u32
{
    unsigned_byte  = 0x1401, // GL_UNSIGNED_BYTE
    unsigned_int   = 0x1405, // GL_UNSIGNED_INT
    unsigned_short = 0x1403, // GL_UNSIGNED_SHORT
}; // enum class draw_elements_type

} // namespace gl::group
