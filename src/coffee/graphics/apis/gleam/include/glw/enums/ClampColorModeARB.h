#pragma once

#include "common.h"

namespace gl::group {

// ClampColorModeARB
enum class clamp_color_mode_arb : u32
{
    false_         = 0,      // GL_FALSE
    fixed_only     = 0x891D, // GL_FIXED_ONLY
    true_          = 1,      // GL_TRUE
    fixed_only_arb = 0x891D, // GL_FIXED_ONLY_ARB
}; // enum class clamp_color_mode_arb

} // namespace gl::group
