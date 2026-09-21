#pragma once

#include "common.h"

namespace gl::group {

// ShaderType
enum class shader_type : u32
{
    compute_shader         = 0x91B9, // GL_COMPUTE_SHADER
    fragment_shader        = 0x8B30, // GL_FRAGMENT_SHADER
    geometry_shader        = 0x8DD9, // GL_GEOMETRY_SHADER
    tess_control_shader    = 0x8E88, // GL_TESS_CONTROL_SHADER
    tess_evaluation_shader = 0x8E87, // GL_TESS_EVALUATION_SHADER
    vertex_shader          = 0x8B31, // GL_VERTEX_SHADER
    fragment_shader_arb    = 0x8B30, // GL_FRAGMENT_SHADER_ARB
    vertex_shader_arb      = 0x8B31, // GL_VERTEX_SHADER_ARB
}; // enum class shader_type

} // namespace gl::group
