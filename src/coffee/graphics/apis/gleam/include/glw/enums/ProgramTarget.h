#pragma once

#include "common.h"

namespace gl::group {

// ProgramTarget
enum class program_target : u32
{
    compute_program_nv         = 0x90FB, // GL_COMPUTE_PROGRAM_NV
    fragment_program_arb       = 0x8804, // GL_FRAGMENT_PROGRAM_ARB
    geometry_program_nv        = 0x8C26, // GL_GEOMETRY_PROGRAM_NV
    tess_control_program_nv    = 0x891E, // GL_TESS_CONTROL_PROGRAM_NV
    tess_evaluation_program_nv = 0x891F, // GL_TESS_EVALUATION_PROGRAM_NV
    text_fragment_shader_ati   = 0x8200, // GL_TEXT_FRAGMENT_SHADER_ATI
    vertex_program_arb         = 0x8620, // GL_VERTEX_PROGRAM_ARB
}; // enum class program_target

} // namespace gl::group
