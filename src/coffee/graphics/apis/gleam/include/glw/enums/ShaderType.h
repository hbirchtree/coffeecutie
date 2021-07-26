#pragma once

#include "common.h"

namespace gl::group {

// ShaderType
enum class shader_type : ::libc_types::u32
{
#ifdef GL_COMPUTE_SHADER
    compute_shader = GL_COMPUTE_SHADER,
#endif
#ifdef GL_FRAGMENT_SHADER
    fragment_shader = GL_FRAGMENT_SHADER,
#endif
#ifdef GL_FRAGMENT_SHADER_ARB
    fragment_shader_arb = GL_FRAGMENT_SHADER_ARB,
#endif
#ifdef GL_GEOMETRY_SHADER
    geometry_shader = GL_GEOMETRY_SHADER,
#endif
#ifdef GL_TESS_CONTROL_SHADER
    tess_control_shader = GL_TESS_CONTROL_SHADER,
#endif
#ifdef GL_TESS_EVALUATION_SHADER
    tess_evaluation_shader = GL_TESS_EVALUATION_SHADER,
#endif
#ifdef GL_VERTEX_SHADER
    vertex_shader = GL_VERTEX_SHADER,
#endif
#ifdef GL_VERTEX_SHADER_ARB
    vertex_shader_arb = GL_VERTEX_SHADER_ARB,
#endif
}; // enum class shader_type

} // namespace gl::group
