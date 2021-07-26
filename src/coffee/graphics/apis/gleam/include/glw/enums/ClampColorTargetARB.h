#pragma once

#include "common.h"

namespace gl::group {

// ClampColorTargetARB
enum class clamp_color_target_arb : ::libc_types::u32
{
#ifdef GL_CLAMP_FRAGMENT_COLOR_ARB
    clamp_fragment_color_arb = GL_CLAMP_FRAGMENT_COLOR_ARB,
#endif
#ifdef GL_CLAMP_READ_COLOR
    clamp_read_color = GL_CLAMP_READ_COLOR,
#endif
#ifdef GL_CLAMP_READ_COLOR_ARB
    clamp_read_color_arb = GL_CLAMP_READ_COLOR_ARB,
#endif
#ifdef GL_CLAMP_VERTEX_COLOR_ARB
    clamp_vertex_color_arb = GL_CLAMP_VERTEX_COLOR_ARB,
#endif
}; // enum class clamp_color_target_arb

} // namespace gl::group
