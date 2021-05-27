#pragma once

#include "common.h"

namespace gl::groups {

// HintMode
enum class hint_mode : ::libc_types::u32 {
#ifdef GL_DONT_CARE
    dont_care = GL_DONT_CARE,
#endif
#ifdef GL_FASTEST
    fastest = GL_FASTEST,
#endif
#ifdef GL_NICEST
    nicest = GL_NICEST,
#endif
}; // enum class hint_mode

} // namespace gl::groups
