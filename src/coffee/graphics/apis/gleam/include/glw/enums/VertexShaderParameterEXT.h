#pragma once

#include "common.h"

namespace gl::group {

// VertexShaderParameterEXT
enum class vertex_shader_parameter_ext : u32
{
    current_vertex_ext = 0x87E2, // GL_CURRENT_VERTEX_EXT
    mvp_matrix_ext     = 0x87E3, // GL_MVP_MATRIX_EXT
}; // enum class vertex_shader_parameter_ext

} // namespace gl::group
