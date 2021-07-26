#pragma once

#ifdef GL_NV_texture_border_clamp
#include "../enums/GetTextureParameter.h"
#include "../enums/TextureParameterName.h"
#include "../enums/TextureWrapMode.h"
namespace gl::nv::texture_border_clamp {
using gl::group::get_texture_parameter;
using gl::group::texture_parameter_name;
using gl::group::texture_wrap_mode;
namespace values {
} // namespace values
} // namespace gl::nv::texture_border_clamp
#endif // GL_NV_texture_border_clamp
namespace gl::nv::texture_border_clamp {
constexpr auto name = "GL_NV_texture_border_clamp";
}
