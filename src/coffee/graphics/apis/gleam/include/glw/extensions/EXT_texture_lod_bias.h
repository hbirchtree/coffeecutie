#pragma once

#ifdef GL_EXT_texture_lod_bias
#include "../enums/SamplerParameterF.h"
#include "../enums/TextureParameterName.h"

namespace gl::ext::texture_lod_bias {
using gl::group::sampler_parameter_f;
using gl::group::texture_parameter_name;

namespace values {
constexpr u32 max_texture_lod_bias   = 0x84FD;
constexpr u32 texture_filter_control = 0x8500;
} // namespace values
} // namespace gl::ext::texture_lod_bias
#endif // GL_EXT_texture_lod_bias
namespace gl::ext::texture_lod_bias {
constexpr auto name = "GL_EXT_texture_lod_bias";
}
