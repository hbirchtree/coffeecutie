#pragma once

#include "common.h"

namespace gl::group {

// PathFillMode
enum class path_fill_mode : u32
{
    invert        = 0x150A, // GL_INVERT
    count_down_nv = 0x9089, // GL_COUNT_DOWN_NV
    count_up_nv   = 0x9088, // GL_COUNT_UP_NV
    nv            = 0x9080, // GL_PATH_FILL_MODE_NV
}; // enum class path_fill_mode

} // namespace gl::group
