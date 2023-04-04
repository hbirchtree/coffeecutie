#pragma once

#include "common.h"

namespace gl::group {

// HintMode
enum class hint_mode : u32
{
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

} // namespace gl::group
