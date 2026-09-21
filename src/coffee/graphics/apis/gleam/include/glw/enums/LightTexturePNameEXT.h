#pragma once

#include "common.h"

namespace gl::group {

// LightTexturePNameEXT
enum class light_texture_prop_ext : u32
{
    attenuation_ext        = 0x834D, // GL_ATTENUATION_EXT
    shadow_attenuation_ext = 0x834E, // GL_SHADOW_ATTENUATION_EXT
}; // enum class light_texture_prop_ext

} // namespace gl::group
