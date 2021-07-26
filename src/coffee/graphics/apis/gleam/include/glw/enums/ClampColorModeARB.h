#pragma once

#include "common.h"

namespace gl::group {

// ClampColorModeARB
enum class clamp_color_mode_arb : ::libc_types::u32
{
#ifdef GL_FALSE
    false_ = GL_FALSE,
#endif
#ifdef GL_FIXED_ONLY
    fixed_only = GL_FIXED_ONLY,
#endif
#ifdef GL_FIXED_ONLY_ARB
    fixed_only_arb = GL_FIXED_ONLY_ARB,
#endif
#ifdef GL_TRUE
    true_ = GL_TRUE,
#endif
}; // enum class clamp_color_mode_arb

} // namespace gl::group
