#pragma once

#include "common.h"

namespace gl::group {

// TextureEnvTarget
enum class texture_env_target : u32
{
    point_sprite           = 0x8861, // GL_POINT_SPRITE
    texture_env            = 0x2300, // GL_TEXTURE_ENV
    texture_filter_control = 0x8500, // GL_TEXTURE_FILTER_CONTROL
}; // enum class texture_env_target

} // namespace gl::group
