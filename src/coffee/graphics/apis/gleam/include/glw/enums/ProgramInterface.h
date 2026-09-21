#pragma once

#include "common.h"

namespace gl::group {

// ProgramInterface
enum class program_interface : u32
{
    buffer_variable             = 0x92E5, // GL_BUFFER_VARIABLE
    compute_subroutine          = 0x92ED, // GL_COMPUTE_SUBROUTINE
    compute_subroutine_uniform  = 0x92F3, // GL_COMPUTE_SUBROUTINE_UNIFORM
    fragment_subroutine         = 0x92EC, // GL_FRAGMENT_SUBROUTINE
    fragment_subroutine_uniform = 0x92F2, // GL_FRAGMENT_SUBROUTINE_UNIFORM
    geometry_subroutine         = 0x92EB, // GL_GEOMETRY_SUBROUTINE
    geometry_subroutine_uniform = 0x92F1, // GL_GEOMETRY_SUBROUTINE_UNIFORM
    program_input               = 0x92E3, // GL_PROGRAM_INPUT
    program_output              = 0x92E4, // GL_PROGRAM_OUTPUT
    shader_storage_block        = 0x92E6, // GL_SHADER_STORAGE_BLOCK
    tess_control_subroutine     = 0x92E9, // GL_TESS_CONTROL_SUBROUTINE
    tess_control_subroutine_uniform =
        0x92EF,                          // GL_TESS_CONTROL_SUBROUTINE_UNIFORM
    tess_evaluation_subroutine = 0x92EA, // GL_TESS_EVALUATION_SUBROUTINE
    tess_evaluation_subroutine_uniform =
        0x92F0, // GL_TESS_EVALUATION_SUBROUTINE_UNIFORM
    transform_feedback_buffer  = 0x8C8E, // GL_TRANSFORM_FEEDBACK_BUFFER
    transform_feedback_varying = 0x92F4, // GL_TRANSFORM_FEEDBACK_VARYING
    uniform                    = 0x92E1, // GL_UNIFORM
    uniform_block              = 0x92E2, // GL_UNIFORM_BLOCK
    vertex_subroutine          = 0x92E8, // GL_VERTEX_SUBROUTINE
    vertex_subroutine_uniform  = 0x92EE, // GL_VERTEX_SUBROUTINE_UNIFORM
}; // enum class program_interface

} // namespace gl::group
