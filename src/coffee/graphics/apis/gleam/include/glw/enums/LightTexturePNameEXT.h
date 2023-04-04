#pragma once

#include "common.h"

namespace gl::group {

// LightTexturePNameEXT
enum class light_texture_prop_ext : u32
{
#ifdef GL_ATTENUATION_EXT
    attenuation_ext = GL_ATTENUATION_EXT,
#endif
#ifdef GL_SHADOW_ATTENUATION_EXT
    shadow_attenuation_ext = GL_SHADOW_ATTENUATION_EXT,
#endif
}; // enum class light_texture_prop_ext

} // namespace gl::group
