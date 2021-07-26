#pragma once

#ifdef GL_ARB_texture_filter_anisotropic
#include "../enums/SamplerParameterF.h"
namespace gl::arb::texture_filter_anisotropic {
using gl::group::sampler_parameter_f;
namespace values {
constexpr libc_types::u32 max_texture_max_anisotropy = 0x84FF;
} // namespace values
} // namespace gl::arb::texture_filter_anisotropic
#endif // GL_ARB_texture_filter_anisotropic
namespace gl::arb::texture_filter_anisotropic {
constexpr auto name = "GL_ARB_texture_filter_anisotropic";
}
