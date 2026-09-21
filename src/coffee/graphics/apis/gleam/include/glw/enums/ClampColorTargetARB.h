#pragma once

#include "common.h"

namespace gl::group {

// ClampColorTargetARB
enum class clamp_color_target_arb : u32
{
    clamp_read_color         = 0x891C, // GL_CLAMP_READ_COLOR
    clamp_fragment_color_arb = 0x891B, // GL_CLAMP_FRAGMENT_COLOR_ARB
    clamp_read_color_arb     = 0x891C, // GL_CLAMP_READ_COLOR_ARB
    clamp_vertex_color_arb   = 0x891A, // GL_CLAMP_VERTEX_COLOR_ARB
}; // enum class clamp_color_target_arb

} // namespace gl::group
