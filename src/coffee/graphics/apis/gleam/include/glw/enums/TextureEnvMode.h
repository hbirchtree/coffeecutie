#pragma once

#include "common.h"

namespace gl::group {

// TextureEnvMode
enum class texture_env_mode : u32
{
    add                   = 0x0104, // GL_ADD
    blend                 = 0x0BE2, // GL_BLEND
    combine               = 0x8570, // GL_COMBINE
    decal                 = 0x2101, // GL_DECAL
    modulate              = 0x2100, // GL_MODULATE
    replace               = 0x1E01, // GL_REPLACE
    replace_ext           = 0x8062, // GL_REPLACE_EXT
    texture_env_bias_sgix = 0x80BE, // GL_TEXTURE_ENV_BIAS_SGIX
}; // enum class texture_env_mode

} // namespace gl::group
