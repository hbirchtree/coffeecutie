#pragma once

#include "common.h"

namespace gl::group {

// CopyImageSubDataTarget
enum class copy_image_sub_data_target : u32
{
    renderbuffer                 = 0x8D41, // GL_RENDERBUFFER
    texture_1d                   = 0x0DE0, // GL_TEXTURE_1D
    texture_1d_array             = 0x8C18, // GL_TEXTURE_1D_ARRAY
    texture_2d                   = 0x0DE1, // GL_TEXTURE_2D
    texture_2d_array             = 0x8C1A, // GL_TEXTURE_2D_ARRAY
    texture_2d_multisample       = 0x9100, // GL_TEXTURE_2D_MULTISAMPLE
    texture_2d_multisample_array = 0x9102, // GL_TEXTURE_2D_MULTISAMPLE_ARRAY
    texture_3d                   = 0x806F, // GL_TEXTURE_3D
    texture_cube_map             = 0x8513, // GL_TEXTURE_CUBE_MAP
    texture_cube_map_array       = 0x9009, // GL_TEXTURE_CUBE_MAP_ARRAY
    texture_rectangle            = 0x84F5, // GL_TEXTURE_RECTANGLE
}; // enum class copy_image_sub_data_target

} // namespace gl::group
