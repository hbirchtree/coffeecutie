#pragma once

#include "common.h"

namespace gl::group {

// TextureNormalModeEXT
enum class texture_normal_mode_ext : ::libc_types::u32
{
#ifdef GL_PERTURB_EXT
    perturb_ext = GL_PERTURB_EXT,
#endif
}; // enum class texture_normal_mode_ext

} // namespace gl::group
