#pragma once

#include "common.h"

namespace gl::groups {

// ProgramInterface
enum class program_interface : ::libc_types::u32 {
#ifdef GL_BUFFER_VARIABLE
    buffer_variable = GL_BUFFER_VARIABLE,
#endif
#ifdef GL_COMPUTE_SUBROUTINE
    compute_subroutine = GL_COMPUTE_SUBROUTINE,
#endif
#ifdef GL_COMPUTE_SUBROUTINE_UNIFORM
    compute_subroutine_uniform = GL_COMPUTE_SUBROUTINE_UNIFORM,
#endif
#ifdef GL_FRAGMENT_SUBROUTINE
    fragment_subroutine = GL_FRAGMENT_SUBROUTINE,
#endif
#ifdef GL_FRAGMENT_SUBROUTINE_UNIFORM
    fragment_subroutine_uniform = GL_FRAGMENT_SUBROUTINE_UNIFORM,
#endif
#ifdef GL_GEOMETRY_SUBROUTINE
    geometry_subroutine = GL_GEOMETRY_SUBROUTINE,
#endif
#ifdef GL_GEOMETRY_SUBROUTINE_UNIFORM
    geometry_subroutine_uniform = GL_GEOMETRY_SUBROUTINE_UNIFORM,
#endif
#ifdef GL_PROGRAM_INPUT
    program_input = GL_PROGRAM_INPUT,
#endif
#ifdef GL_PROGRAM_OUTPUT
    program_output = GL_PROGRAM_OUTPUT,
#endif
#ifdef GL_SHADER_STORAGE_BLOCK
    shader_storage_block = GL_SHADER_STORAGE_BLOCK,
#endif
#ifdef GL_TESS_CONTROL_SUBROUTINE
    tess_control_subroutine = GL_TESS_CONTROL_SUBROUTINE,
#endif
#ifdef GL_TESS_CONTROL_SUBROUTINE_UNIFORM
    tess_control_subroutine_uniform = GL_TESS_CONTROL_SUBROUTINE_UNIFORM,
#endif
#ifdef GL_TESS_EVALUATION_SUBROUTINE
    tess_evaluation_subroutine = GL_TESS_EVALUATION_SUBROUTINE,
#endif
#ifdef GL_TESS_EVALUATION_SUBROUTINE_UNIFORM
    tess_evaluation_subroutine_uniform = GL_TESS_EVALUATION_SUBROUTINE_UNIFORM,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER
    transform_feedback_buffer = GL_TRANSFORM_FEEDBACK_BUFFER,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_VARYING
    transform_feedback_varying = GL_TRANSFORM_FEEDBACK_VARYING,
#endif
#ifdef GL_UNIFORM
    uniform = GL_UNIFORM,
#endif
#ifdef GL_UNIFORM_BLOCK
    uniform_block = GL_UNIFORM_BLOCK,
#endif
#ifdef GL_VERTEX_SUBROUTINE
    vertex_subroutine = GL_VERTEX_SUBROUTINE,
#endif
#ifdef GL_VERTEX_SUBROUTINE_UNIFORM
    vertex_subroutine_uniform = GL_VERTEX_SUBROUTINE_UNIFORM,
#endif
}; // enum class program_interface

} // namespace gl::groups
