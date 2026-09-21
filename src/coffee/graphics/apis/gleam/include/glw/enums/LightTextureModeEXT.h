#pragma once

#include "common.h"

namespace gl::group {

// LightTextureModeEXT
enum class light_texture_mode_ext : u32
{
    fragment_depth        = 0x8452, // GL_FRAGMENT_DEPTH
    fragment_color_ext    = 0x834C, // GL_FRAGMENT_COLOR_EXT
    fragment_depth_ext    = 0x8452, // GL_FRAGMENT_DEPTH_EXT
    fragment_material_ext = 0x8349, // GL_FRAGMENT_MATERIAL_EXT
    fragment_normal_ext   = 0x834A, // GL_FRAGMENT_NORMAL_EXT
}; // enum class light_texture_mode_ext

} // namespace gl::group
