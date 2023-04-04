#pragma once

#include "common.h"

namespace gl::group {

// LightModelColorControl
enum class light_model_color_control : u32
{
#ifdef GL_SEPARATE_SPECULAR_COLOR
    separate_specular_color = GL_SEPARATE_SPECULAR_COLOR,
#endif
#ifdef GL_SEPARATE_SPECULAR_COLOR_EXT
    separate_specular_color_ext = GL_SEPARATE_SPECULAR_COLOR_EXT,
#endif
#ifdef GL_SINGLE_COLOR
    single_color = GL_SINGLE_COLOR,
#endif
#ifdef GL_SINGLE_COLOR_EXT
    single_color_ext = GL_SINGLE_COLOR_EXT,
#endif
}; // enum class light_model_color_control

} // namespace gl::group
