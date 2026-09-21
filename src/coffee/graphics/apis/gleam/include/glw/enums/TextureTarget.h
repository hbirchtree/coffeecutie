#pragma once

#include "common.h"

namespace gl::group {

// TextureTarget
enum class texture_target : u32
{
    proxy_texture_1d             = 0x8063, // GL_PROXY_TEXTURE_1D
    proxy_texture_1d_array       = 0x8C19, // GL_PROXY_TEXTURE_1D_ARRAY
    proxy_texture_2d             = 0x8064, // GL_PROXY_TEXTURE_2D
    proxy_texture_2d_array       = 0x8C1B, // GL_PROXY_TEXTURE_2D_ARRAY
    proxy_texture_2d_multisample = 0x9101, // GL_PROXY_TEXTURE_2D_MULTISAMPLE
    proxy_texture_2d_multisample_array =
        0x9103, // GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY
    proxy_texture_3d             = 0x8070, // GL_PROXY_TEXTURE_3D
    proxy_texture_cube_map       = 0x851B, // GL_PROXY_TEXTURE_CUBE_MAP
    proxy_texture_cube_map_array = 0x900B, // GL_PROXY_TEXTURE_CUBE_MAP_ARRAY
    proxy_texture_rectangle      = 0x84F7, // GL_PROXY_TEXTURE_RECTANGLE
    renderbuffer                 = 0x8D41, // GL_RENDERBUFFER
    texture_1d                   = 0x0DE0, // GL_TEXTURE_1D
    texture_1d_array             = 0x8C18, // GL_TEXTURE_1D_ARRAY
    texture_2d                   = 0x0DE1, // GL_TEXTURE_2D
    texture_2d_array             = 0x8C1A, // GL_TEXTURE_2D_ARRAY
    texture_2d_multisample       = 0x9100, // GL_TEXTURE_2D_MULTISAMPLE
    texture_2d_multisample_array = 0x9102, // GL_TEXTURE_2D_MULTISAMPLE_ARRAY
    texture_3d                   = 0x806F, // GL_TEXTURE_3D
    texture_buffer               = 0x8C2A, // GL_TEXTURE_BUFFER
    texture_cube_map             = 0x8513, // GL_TEXTURE_CUBE_MAP
    texture_cube_map_array       = 0x9009, // GL_TEXTURE_CUBE_MAP_ARRAY
    texture_cube_map_negative_x  = 0x8516, // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
    texture_cube_map_negative_y  = 0x8518, // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
    texture_cube_map_negative_z  = 0x851A, // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    texture_cube_map_positive_x  = 0x8515, // GL_TEXTURE_CUBE_MAP_POSITIVE_X
    texture_cube_map_positive_y  = 0x8517, // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
    texture_cube_map_positive_z  = 0x8519, // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
    texture_rectangle            = 0x84F5, // GL_TEXTURE_RECTANGLE
    detail_texture_2d_sgis       = 0x8095, // GL_DETAIL_TEXTURE_2D_SGIS
    proxy_texture_1d_array_ext   = 0x8C19, // GL_PROXY_TEXTURE_1D_ARRAY_EXT
    proxy_texture_1d_ext         = 0x8063, // GL_PROXY_TEXTURE_1D_EXT
    proxy_texture_2d_array_ext   = 0x8C1B, // GL_PROXY_TEXTURE_2D_ARRAY_EXT
    proxy_texture_2d_ext         = 0x8064, // GL_PROXY_TEXTURE_2D_EXT
    proxy_texture_3d_ext         = 0x8070, // GL_PROXY_TEXTURE_3D_EXT
    proxy_texture_4d_sgis        = 0x8135, // GL_PROXY_TEXTURE_4D_SGIS
    proxy_texture_cube_map_arb   = 0x851B, // GL_PROXY_TEXTURE_CUBE_MAP_ARB
    proxy_texture_cube_map_array_arb =
        0x900B,                           // GL_PROXY_TEXTURE_CUBE_MAP_ARRAY_ARB
    proxy_texture_cube_map_ext  = 0x851B, // GL_PROXY_TEXTURE_CUBE_MAP_EXT
    proxy_texture_rectangle_arb = 0x84F7, // GL_PROXY_TEXTURE_RECTANGLE_ARB
    proxy_texture_rectangle_nv  = 0x84F7, // GL_PROXY_TEXTURE_RECTANGLE_NV
    texture_3d_ext              = 0x806F, // GL_TEXTURE_3D_EXT
    texture_3d_oes              = 0x806F, // GL_TEXTURE_3D_OES
    texture_4d_sgis             = 0x8134, // GL_TEXTURE_4D_SGIS
    texture_cube_map_arb        = 0x8513, // GL_TEXTURE_CUBE_MAP_ARB
    texture_cube_map_array_arb  = 0x9009, // GL_TEXTURE_CUBE_MAP_ARRAY_ARB
    texture_cube_map_array_ext  = 0x9009, // GL_TEXTURE_CUBE_MAP_ARRAY_EXT
    texture_cube_map_array_oes  = 0x9009, // GL_TEXTURE_CUBE_MAP_ARRAY_OES
    texture_cube_map_ext        = 0x8513, // GL_TEXTURE_CUBE_MAP_EXT
    texture_cube_map_negative_x_arb =
        0x8516, // GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB
    texture_cube_map_negative_x_ext =
        0x8516, // GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT
    texture_cube_map_negative_x_oes =
        0x8516, // GL_TEXTURE_CUBE_MAP_NEGATIVE_X_OES
    texture_cube_map_negative_y_arb =
        0x8518, // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB
    texture_cube_map_negative_y_ext =
        0x8518, // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT
    texture_cube_map_negative_y_oes =
        0x8518, // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_OES
    texture_cube_map_negative_z_arb =
        0x851A, // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB
    texture_cube_map_negative_z_ext =
        0x851A, // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT
    texture_cube_map_negative_z_oes =
        0x851A,                    // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_OES
    texture_cube_map_oes = 0x8513, // GL_TEXTURE_CUBE_MAP_OES
    texture_cube_map_positive_x_arb =
        0x8515, // GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB
    texture_cube_map_positive_x_ext =
        0x8515, // GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT
    texture_cube_map_positive_x_oes =
        0x8515, // GL_TEXTURE_CUBE_MAP_POSITIVE_X_OES
    texture_cube_map_positive_y_arb =
        0x8517, // GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB
    texture_cube_map_positive_y_ext =
        0x8517, // GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT
    texture_cube_map_positive_y_oes =
        0x8517, // GL_TEXTURE_CUBE_MAP_POSITIVE_Y_OES
    texture_cube_map_positive_z_arb =
        0x8519, // GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB
    texture_cube_map_positive_z_ext =
        0x8519, // GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT
    texture_cube_map_positive_z_oes =
        0x8519,                     // GL_TEXTURE_CUBE_MAP_POSITIVE_Z_OES
    texture_rectangle_arb = 0x84F5, // GL_TEXTURE_RECTANGLE_ARB
    texture_rectangle_nv  = 0x84F5, // GL_TEXTURE_RECTANGLE_NV
}; // enum class texture_target

} // namespace gl::group
