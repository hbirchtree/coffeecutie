#pragma once

#include "common.h"

namespace gl::group {

// VertexShaderParameterEXT
enum class vertex_shader_parameter_ext : ::libc_types::u32
{
#ifdef GL_CURRENT_VERTEX_EXT
    current_vertex_ext = GL_CURRENT_VERTEX_EXT,
#endif
#ifdef GL_MVP_MATRIX_EXT
    mvp_matrix_ext = GL_MVP_MATRIX_EXT,
#endif
}; // enum class vertex_shader_parameter_ext

} // namespace gl::group
