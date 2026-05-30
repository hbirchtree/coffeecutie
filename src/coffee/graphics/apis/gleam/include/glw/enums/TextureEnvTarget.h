#pragma once

#include "common.h"

namespace gl::group {

// TextureEnvTarget
enum class texture_env_target : u32
{
#ifdef GL_POINT_SPRITE
    point_sprite = GL_POINT_SPRITE,
#endif
#ifdef GL_TEXTURE_ENV
    texture_env = GL_TEXTURE_ENV,
#endif
#ifdef GL_TEXTURE_FILTER_CONTROL
    texture_filter_control = GL_TEXTURE_FILTER_CONTROL,
#endif
}; // enum class texture_env_target

} // namespace gl::group
