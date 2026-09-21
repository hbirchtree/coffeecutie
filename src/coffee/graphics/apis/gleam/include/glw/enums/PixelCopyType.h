#pragma once

#include "common.h"

namespace gl::group {

// PixelCopyType
enum class pixel_copy_type : u32
{
    color       = 0x1800, // GL_COLOR
    depth       = 0x1801, // GL_DEPTH
    stencil     = 0x1802, // GL_STENCIL
    color_ext   = 0x1800, // GL_COLOR_EXT
    depth_ext   = 0x1801, // GL_DEPTH_EXT
    stencil_ext = 0x1802, // GL_STENCIL_EXT
}; // enum class pixel_copy_type

} // namespace gl::group
