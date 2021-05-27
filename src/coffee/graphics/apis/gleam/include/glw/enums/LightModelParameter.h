#pragma once

#include "common.h"

namespace gl::groups {

// LightModelParameter
enum class light_model_parameter : ::libc_types::u32 {
#ifdef GL_LIGHT_MODEL_AMBIENT
    light_model_ambient = GL_LIGHT_MODEL_AMBIENT,
#endif
#ifdef GL_LIGHT_MODEL_COLOR_CONTROL
    light_model_color_control = GL_LIGHT_MODEL_COLOR_CONTROL,
#endif
#ifdef GL_LIGHT_MODEL_COLOR_CONTROL_EXT
    light_model_color_control_ext = GL_LIGHT_MODEL_COLOR_CONTROL_EXT,
#endif
#ifdef GL_LIGHT_MODEL_LOCAL_VIEWER
    light_model_local_viewer = GL_LIGHT_MODEL_LOCAL_VIEWER,
#endif
#ifdef GL_LIGHT_MODEL_TWO_SIDE
    light_model_two_side = GL_LIGHT_MODEL_TWO_SIDE,
#endif
}; // enum class light_model_parameter

} // namespace gl::groups
