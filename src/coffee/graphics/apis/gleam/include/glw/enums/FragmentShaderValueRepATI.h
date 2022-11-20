#pragma once

#include "common.h"

namespace gl::group {

// FragmentShaderValueRepATI
enum class fragment_shader_value_rep_ati : ::libc_types::u32
{
#ifdef GL_ALPHA
    alpha = GL_ALPHA,
#endif
#ifdef GL_BLUE
    blue = GL_BLUE,
#endif
#ifdef GL_GREEN
    green = GL_GREEN,
#endif
#ifdef GL_NONE
    none = GL_NONE,
#endif
#ifdef GL_RED
    red = GL_RED,
#endif
}; // enum class fragment_shader_value_rep_ati

} // namespace gl::group
