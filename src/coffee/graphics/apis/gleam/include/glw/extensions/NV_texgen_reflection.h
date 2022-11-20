#pragma once

#ifdef GL_NV_texgen_reflection
#include "../enums/GetTextureParameter.h"
namespace gl::nv::texgen_reflection {
using gl::group::get_texture_parameter;
namespace values {
} // namespace values
} // namespace gl::nv::texgen_reflection
#endif // GL_NV_texgen_reflection
namespace gl::nv::texgen_reflection {
constexpr auto name = "GL_NV_texgen_reflection";
}
