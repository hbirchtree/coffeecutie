#pragma once

#include "common.h"

namespace gl::group {

// LightTextureModeEXT
enum class light_texture_mode_ext : u32
{
#ifdef GL_FRAGMENT_COLOR_EXT
    fragment_color_ext = GL_FRAGMENT_COLOR_EXT,
#endif
#ifdef GL_FRAGMENT_DEPTH
    fragment_depth = GL_FRAGMENT_DEPTH,
#endif
#ifdef GL_FRAGMENT_DEPTH_EXT
    fragment_depth_ext = GL_FRAGMENT_DEPTH_EXT,
#endif
#ifdef GL_FRAGMENT_MATERIAL_EXT
    fragment_material_ext = GL_FRAGMENT_MATERIAL_EXT,
#endif
#ifdef GL_FRAGMENT_NORMAL_EXT
    fragment_normal_ext = GL_FRAGMENT_NORMAL_EXT,
#endif
}; // enum class light_texture_mode_ext

} // namespace gl::group
