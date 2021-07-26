#pragma once

#include "common.h"

namespace gl::group {

// CopyImageSubDataTarget
enum class copy_image_sub_data_target : ::libc_types::u32
{
#ifdef GL_RENDERBUFFER
    renderbuffer = GL_RENDERBUFFER,
#endif
#ifdef GL_TEXTURE_1D
    texture_1d = GL_TEXTURE_1D,
#endif
#ifdef GL_TEXTURE_1D_ARRAY
    texture_1d_array = GL_TEXTURE_1D_ARRAY,
#endif
#ifdef GL_TEXTURE_2D
    texture_2d = GL_TEXTURE_2D,
#endif
#ifdef GL_TEXTURE_2D_ARRAY
    texture_2d_array = GL_TEXTURE_2D_ARRAY,
#endif
#ifdef GL_TEXTURE_2D_MULTISAMPLE
    texture_2d_multisample = GL_TEXTURE_2D_MULTISAMPLE,
#endif
#ifdef GL_TEXTURE_2D_MULTISAMPLE_ARRAY
    texture_2d_multisample_array = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
#endif
#ifdef GL_TEXTURE_3D
    texture_3d = GL_TEXTURE_3D,
#endif
#ifdef GL_TEXTURE_CUBE_MAP
    texture_cube_map = GL_TEXTURE_CUBE_MAP,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_ARRAY
    texture_cube_map_array = GL_TEXTURE_CUBE_MAP_ARRAY,
#endif
#ifdef GL_TEXTURE_RECTANGLE
    texture_rectangle = GL_TEXTURE_RECTANGLE,
#endif
}; // enum class copy_image_sub_data_target

} // namespace gl::group
