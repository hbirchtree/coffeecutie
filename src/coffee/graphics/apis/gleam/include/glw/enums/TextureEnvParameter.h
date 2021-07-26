#pragma once

#include "common.h"

namespace gl::group {

// TextureEnvParameter
enum class texture_env_parameter : ::libc_types::u32
{
#ifdef GL_TEXTURE_ENV_COLOR
    texture_env_color = GL_TEXTURE_ENV_COLOR,
#endif
#ifdef GL_TEXTURE_ENV_MODE
    texture_env_mode = GL_TEXTURE_ENV_MODE,
#endif
}; // enum class texture_env_parameter

} // namespace gl::group
