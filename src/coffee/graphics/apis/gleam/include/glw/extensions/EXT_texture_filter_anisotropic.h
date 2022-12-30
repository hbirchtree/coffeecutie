#pragma once

#ifdef GL_EXT_texture_filter_anisotropic
#include "../enums/GetPName.h"
#include "../enums/SamplerParameterF.h"
#include "../enums/TextureParameterName.h"
namespace gl::ext::texture_filter_anisotropic {
using gl::group::get_prop;
using gl::group::sampler_parameter_f;
using gl::group::texture_parameter_name;
namespace values {
} // namespace values
} // namespace gl::ext::texture_filter_anisotropic
#endif // GL_EXT_texture_filter_anisotropic
namespace gl::ext::texture_filter_anisotropic {
constexpr auto name = "GL_EXT_texture_filter_anisotropic";
}
