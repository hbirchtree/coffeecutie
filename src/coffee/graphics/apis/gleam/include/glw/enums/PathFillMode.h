#pragma once

#include "common.h"

namespace gl::groups {

// PathFillMode
enum class path_fill_mode : ::libc_types::u32 {
#ifdef GL_COUNT_DOWN_NV
    count_down_nv = GL_COUNT_DOWN_NV,
#endif
#ifdef GL_COUNT_UP_NV
    count_up_nv = GL_COUNT_UP_NV,
#endif
#ifdef GL_INVERT
    invert = GL_INVERT,
#endif
#ifdef GL_PATH_FILL_MODE_NV
    nv = GL_PATH_FILL_MODE_NV,
#endif
}; // enum class path_fill_mode

} // namespace gl::groups
