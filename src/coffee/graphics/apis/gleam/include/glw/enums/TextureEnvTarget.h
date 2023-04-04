#pragma once

#include "common.h"

namespace gl::group {

// TextureEnvTarget
enum class texture_env_target : u32
{
#ifdef GL_TEXTURE_ENV
    texture_env = GL_TEXTURE_ENV,
#endif
}; // enum class texture_env_target

} // namespace gl::group
