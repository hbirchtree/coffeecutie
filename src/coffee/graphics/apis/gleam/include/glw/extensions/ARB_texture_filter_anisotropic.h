#pragma once

#ifdef GL_ARB_texture_filter_anisotropic
#include "../enums/GetPName.h"
#include "../enums/SamplerParameterF.h"
#include "../enums/TextureParameterName.h"
namespace gl::arb::texture_filter_anisotropic {
using gl::group::get_prop;
using gl::group::sampler_parameter_f;
using gl::group::texture_parameter_name;
namespace values {
} // namespace values
} // namespace gl::arb::texture_filter_anisotropic
#endif // GL_ARB_texture_filter_anisotropic
namespace gl::arb::texture_filter_anisotropic {
constexpr auto name = "GL_ARB_texture_filter_anisotropic";
}
