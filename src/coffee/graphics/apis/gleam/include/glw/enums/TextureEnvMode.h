#pragma once

#include "common.h"

namespace gl::group {

// TextureEnvMode
enum class texture_env_mode : ::libc_types::u32
{
#ifdef GL_ADD
    add = GL_ADD,
#endif
#ifdef GL_BLEND
    blend = GL_BLEND,
#endif
#ifdef GL_DECAL
    decal = GL_DECAL,
#endif
#ifdef GL_MODULATE
    modulate = GL_MODULATE,
#endif
#ifdef GL_REPLACE_EXT
    replace_ext = GL_REPLACE_EXT,
#endif
#ifdef GL_TEXTURE_ENV_BIAS_SGIX
    texture_env_bias_sgix = GL_TEXTURE_ENV_BIAS_SGIX,
#endif
}; // enum class texture_env_mode

} // namespace gl::group
