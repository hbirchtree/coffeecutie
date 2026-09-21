#pragma once

#include "common.h"

namespace gl::group {

// FragmentShaderValueRepATI
enum class fragment_shader_value_rep_ati : u32
{
    alpha = 0x1906, // GL_ALPHA
    blue  = 0x1905, // GL_BLUE
    green = 0x1904, // GL_GREEN
    none  = 0,      // GL_NONE
    red   = 0x1903, // GL_RED
}; // enum class fragment_shader_value_rep_ati

} // namespace gl::group
