#pragma once

#include "common.h"

namespace gl::group {

// VertexShaderCoordOutEXT
enum class vertex_shader_coord_out_ext : u32
{
    negative_one_ext = 0x87DF, // GL_NEGATIVE_ONE_EXT
    negative_w_ext   = 0x87DC, // GL_NEGATIVE_W_EXT
    negative_x_ext   = 0x87D9, // GL_NEGATIVE_X_EXT
    negative_y_ext   = 0x87DA, // GL_NEGATIVE_Y_EXT
    negative_z_ext   = 0x87DB, // GL_NEGATIVE_Z_EXT
    one_ext          = 0x87DE, // GL_ONE_EXT
    w_ext            = 0x87D8, // GL_W_EXT
    x_ext            = 0x87D5, // GL_X_EXT
    y_ext            = 0x87D6, // GL_Y_EXT
    zero_ext         = 0x87DD, // GL_ZERO_EXT
    z_ext            = 0x87D7, // GL_Z_EXT
}; // enum class vertex_shader_coord_out_ext

} // namespace gl::group
