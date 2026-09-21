#pragma once

#include "common.h"

namespace gl::group {

// ReplacementCodeTypeSUN
enum class replacement_code_type_sun : u32
{
    unsigned_byte  = 0x1401, // GL_UNSIGNED_BYTE
    unsigned_int   = 0x1405, // GL_UNSIGNED_INT
    unsigned_short = 0x1403, // GL_UNSIGNED_SHORT
}; // enum class replacement_code_type_sun

} // namespace gl::group
