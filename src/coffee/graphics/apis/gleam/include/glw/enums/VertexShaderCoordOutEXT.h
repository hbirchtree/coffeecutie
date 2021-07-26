#pragma once

#include "common.h"

namespace gl::group {

// VertexShaderCoordOutEXT
enum class vertex_shader_coord_out_ext : ::libc_types::u32
{
#ifdef GL_NEGATIVE_ONE_EXT
    negative_one_ext = GL_NEGATIVE_ONE_EXT,
#endif
#ifdef GL_NEGATIVE_W_EXT
    negative_w_ext = GL_NEGATIVE_W_EXT,
#endif
#ifdef GL_NEGATIVE_X_EXT
    negative_x_ext = GL_NEGATIVE_X_EXT,
#endif
#ifdef GL_NEGATIVE_Y_EXT
    negative_y_ext = GL_NEGATIVE_Y_EXT,
#endif
#ifdef GL_NEGATIVE_Z_EXT
    negative_z_ext = GL_NEGATIVE_Z_EXT,
#endif
#ifdef GL_ONE_EXT
    one_ext = GL_ONE_EXT,
#endif
#ifdef GL_W_EXT
    w_ext = GL_W_EXT,
#endif
#ifdef GL_X_EXT
    x_ext = GL_X_EXT,
#endif
#ifdef GL_Y_EXT
    y_ext = GL_Y_EXT,
#endif
#ifdef GL_ZERO_EXT
    zero_ext = GL_ZERO_EXT,
#endif
#ifdef GL_Z_EXT
    z_ext = GL_Z_EXT,
#endif
}; // enum class vertex_shader_coord_out_ext

} // namespace gl::group
