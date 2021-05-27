#pragma once

#include "common.h"

namespace gl::groups {

// TextureTarget
enum class texture_target : ::libc_types::u32 {
#ifdef GL_DETAIL_TEXTURE_2D_SGIS
    detail_texture_2d_sgis = GL_DETAIL_TEXTURE_2D_SGIS,
#endif
#ifdef GL_PROXY_TEXTURE_1D
    proxy_texture_1d = GL_PROXY_TEXTURE_1D,
#endif
#ifdef GL_PROXY_TEXTURE_1D_ARRAY
    proxy_texture_1d_array = GL_PROXY_TEXTURE_1D_ARRAY,
#endif
#ifdef GL_PROXY_TEXTURE_1D_ARRAY_EXT
    proxy_texture_1d_array_ext = GL_PROXY_TEXTURE_1D_ARRAY_EXT,
#endif
#ifdef GL_PROXY_TEXTURE_1D_EXT
    proxy_texture_1d_ext = GL_PROXY_TEXTURE_1D_EXT,
#endif
#ifdef GL_PROXY_TEXTURE_2D
    proxy_texture_2d = GL_PROXY_TEXTURE_2D,
#endif
#ifdef GL_PROXY_TEXTURE_2D_ARRAY
    proxy_texture_2d_array = GL_PROXY_TEXTURE_2D_ARRAY,
#endif
#ifdef GL_PROXY_TEXTURE_2D_ARRAY_EXT
    proxy_texture_2d_array_ext = GL_PROXY_TEXTURE_2D_ARRAY_EXT,
#endif
#ifdef GL_PROXY_TEXTURE_2D_EXT
    proxy_texture_2d_ext = GL_PROXY_TEXTURE_2D_EXT,
#endif
#ifdef GL_PROXY_TEXTURE_2D_MULTISAMPLE
    proxy_texture_2d_multisample = GL_PROXY_TEXTURE_2D_MULTISAMPLE,
#endif
#ifdef GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY
    proxy_texture_2d_multisample_array = GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY,
#endif
#ifdef GL_PROXY_TEXTURE_3D
    proxy_texture_3d = GL_PROXY_TEXTURE_3D,
#endif
#ifdef GL_PROXY_TEXTURE_3D_EXT
    proxy_texture_3d_ext = GL_PROXY_TEXTURE_3D_EXT,
#endif
#ifdef GL_PROXY_TEXTURE_4D_SGIS
    proxy_texture_4d_sgis = GL_PROXY_TEXTURE_4D_SGIS,
#endif
#ifdef GL_PROXY_TEXTURE_CUBE_MAP
    proxy_texture_cube_map = GL_PROXY_TEXTURE_CUBE_MAP,
#endif
#ifdef GL_PROXY_TEXTURE_CUBE_MAP_ARB
    proxy_texture_cube_map_arb = GL_PROXY_TEXTURE_CUBE_MAP_ARB,
#endif
#ifdef GL_PROXY_TEXTURE_CUBE_MAP_ARRAY
    proxy_texture_cube_map_array = GL_PROXY_TEXTURE_CUBE_MAP_ARRAY,
#endif
#ifdef GL_PROXY_TEXTURE_CUBE_MAP_ARRAY_ARB
    proxy_texture_cube_map_array_arb = GL_PROXY_TEXTURE_CUBE_MAP_ARRAY_ARB,
#endif
#ifdef GL_PROXY_TEXTURE_CUBE_MAP_EXT
    proxy_texture_cube_map_ext = GL_PROXY_TEXTURE_CUBE_MAP_EXT,
#endif
#ifdef GL_PROXY_TEXTURE_RECTANGLE
    proxy_texture_rectangle = GL_PROXY_TEXTURE_RECTANGLE,
#endif
#ifdef GL_PROXY_TEXTURE_RECTANGLE_ARB
    proxy_texture_rectangle_arb = GL_PROXY_TEXTURE_RECTANGLE_ARB,
#endif
#ifdef GL_PROXY_TEXTURE_RECTANGLE_NV
    proxy_texture_rectangle_nv = GL_PROXY_TEXTURE_RECTANGLE_NV,
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
#ifdef GL_TEXTURE_3D_EXT
    texture_3d_ext = GL_TEXTURE_3D_EXT,
#endif
#ifdef GL_TEXTURE_3D_OES
    texture_3d_oes = GL_TEXTURE_3D_OES,
#endif
#ifdef GL_TEXTURE_4D_SGIS
    texture_4d_sgis = GL_TEXTURE_4D_SGIS,
#endif
#ifdef GL_TEXTURE_BUFFER
    texture_buffer = GL_TEXTURE_BUFFER,
#endif
#ifdef GL_TEXTURE_CUBE_MAP
    texture_cube_map = GL_TEXTURE_CUBE_MAP,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_ARRAY
    texture_cube_map_array = GL_TEXTURE_CUBE_MAP_ARRAY,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_ARRAY_ARB
    texture_cube_map_array_arb = GL_TEXTURE_CUBE_MAP_ARRAY_ARB,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_ARRAY_EXT
    texture_cube_map_array_ext = GL_TEXTURE_CUBE_MAP_ARRAY_EXT,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_ARRAY_OES
    texture_cube_map_array_oes = GL_TEXTURE_CUBE_MAP_ARRAY_OES,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_NEGATIVE_X
    texture_cube_map_negative_x = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
    texture_cube_map_negative_y = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    texture_cube_map_negative_z = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_POSITIVE_X
    texture_cube_map_positive_x = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_POSITIVE_Y
    texture_cube_map_positive_y = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
#endif
#ifdef GL_TEXTURE_CUBE_MAP_POSITIVE_Z
    texture_cube_map_positive_z = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
#endif
#ifdef GL_TEXTURE_RECTANGLE
    texture_rectangle = GL_TEXTURE_RECTANGLE,
#endif
}; // enum class texture_target

} // namespace gl::groups
