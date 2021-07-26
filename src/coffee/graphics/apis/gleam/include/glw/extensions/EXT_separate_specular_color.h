#pragma once

#ifdef GL_EXT_separate_specular_color
#include "../enums/LightModelColorControl.h"
#include "../enums/LightModelParameter.h"
namespace gl::ext::separate_specular_color {
using gl::group::light_model_color_control;
using gl::group::light_model_parameter;
namespace values {
} // namespace values
} // namespace gl::ext::separate_specular_color
#endif // GL_EXT_separate_specular_color
namespace gl::ext::separate_specular_color {
constexpr auto name = "GL_EXT_separate_specular_color";
}
