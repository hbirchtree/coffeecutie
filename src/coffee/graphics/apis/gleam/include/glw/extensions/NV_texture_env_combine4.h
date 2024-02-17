#pragma once

#ifdef GL_NV_texture_env_combine4
#include "../enums/TextureEnvParameter.h"

namespace gl::nv::texture_env_combine4 {
using gl::group::texture_env_parameter;

namespace values {
constexpr u32 combine4 = 0x8503;
} // namespace values
} // namespace gl::nv::texture_env_combine4
#endif // GL_NV_texture_env_combine4
namespace gl::nv::texture_env_combine4 {
constexpr auto name = "GL_NV_texture_env_combine4";
}
