#pragma once

#include "common.h"

namespace gl::group {

// PixelCopyType
enum class pixel_copy_type : u32
{
#ifdef GL_COLOR
    color = GL_COLOR,
#endif
#ifdef GL_COLOR_EXT
    color_ext = GL_COLOR_EXT,
#endif
#ifdef GL_DEPTH
    depth = GL_DEPTH,
#endif
#ifdef GL_DEPTH_EXT
    depth_ext = GL_DEPTH_EXT,
#endif
#ifdef GL_STENCIL
    stencil = GL_STENCIL,
#endif
#ifdef GL_STENCIL_EXT
    stencil_ext = GL_STENCIL_EXT,
#endif
}; // enum class pixel_copy_type

} // namespace gl::group
