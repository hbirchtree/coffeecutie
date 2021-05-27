#pragma once

#include "common.h"

namespace gl::groups {

// TextureEnvTarget
enum class texture_env_target : ::libc_types::u32 {
#ifdef GL_TEXTURE_ENV
    texture_env = GL_TEXTURE_ENV,
#endif
}; // enum class texture_env_target

} // namespace gl::groups
