#pragma once

#ifdef GL_ARB_texture_env_combine
#include "../enums/FragmentShaderGenericSourceATI.h"
#include "../enums/TextureEnvParameter.h"
namespace gl::arb::texture_env_combine {
using gl::group::fragment_shader_generic_source_ati;
using gl::group::texture_env_parameter;
namespace values {
constexpr u32 subtract = 0x84E7;
} // namespace values
} // namespace gl::arb::texture_env_combine
#endif // GL_ARB_texture_env_combine
namespace gl::arb::texture_env_combine {
constexpr auto name = "GL_ARB_texture_env_combine";
}
