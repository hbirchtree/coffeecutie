#pragma once

#include "common.h"

namespace gl::group {

// LightModelColorControl
enum class light_model_color_control : u32
{
    separate_specular_color     = 0x81FA, // GL_SEPARATE_SPECULAR_COLOR
    single_color                = 0x81F9, // GL_SINGLE_COLOR
    separate_specular_color_ext = 0x81FA, // GL_SEPARATE_SPECULAR_COLOR_EXT
    single_color_ext            = 0x81F9, // GL_SINGLE_COLOR_EXT
}; // enum class light_model_color_control

} // namespace gl::group
