#pragma once

#include "common.h"

namespace gl::group {

// MatrixMode
enum class matrix_mode : u32
{
    modelview      = 0x1700, // GL_MODELVIEW
    projection     = 0x1701, // GL_PROJECTION
    texture        = 0x1702, // GL_TEXTURE
    modelview0_ext = 0x1700, // GL_MODELVIEW0_EXT
}; // enum class matrix_mode

} // namespace gl::group
