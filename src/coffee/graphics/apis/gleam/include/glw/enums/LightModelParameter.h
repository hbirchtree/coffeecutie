#pragma once

#include "common.h"

namespace gl::group {

// LightModelParameter
enum class light_model_parameter : u32
{
    light_model_ambient           = 0x0B53, // GL_LIGHT_MODEL_AMBIENT
    light_model_color_control     = 0x81F8, // GL_LIGHT_MODEL_COLOR_CONTROL
    light_model_local_viewer      = 0x0B51, // GL_LIGHT_MODEL_LOCAL_VIEWER
    light_model_two_side          = 0x0B52, // GL_LIGHT_MODEL_TWO_SIDE
    light_model_color_control_ext = 0x81F8, // GL_LIGHT_MODEL_COLOR_CONTROL_EXT
}; // enum class light_model_parameter

} // namespace gl::group
