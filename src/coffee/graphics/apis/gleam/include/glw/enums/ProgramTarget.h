#pragma once

#include "common.h"

namespace gl::group {

// ProgramTarget
enum class program_target : ::libc_types::u32
{
#ifdef GL_COMPUTE_PROGRAM_NV
    compute_program_nv = GL_COMPUTE_PROGRAM_NV,
#endif
#ifdef GL_FRAGMENT_PROGRAM_ARB
    fragment_program_arb = GL_FRAGMENT_PROGRAM_ARB,
#endif
#ifdef GL_GEOMETRY_PROGRAM_NV
    geometry_program_nv = GL_GEOMETRY_PROGRAM_NV,
#endif
#ifdef GL_TESS_CONTROL_PROGRAM_NV
    tess_control_program_nv = GL_TESS_CONTROL_PROGRAM_NV,
#endif
#ifdef GL_TESS_EVALUATION_PROGRAM_NV
    tess_evaluation_program_nv = GL_TESS_EVALUATION_PROGRAM_NV,
#endif
#ifdef GL_TEXT_FRAGMENT_SHADER_ATI
    text_fragment_shader_ati = GL_TEXT_FRAGMENT_SHADER_ATI,
#endif
#ifdef GL_VERTEX_PROGRAM_ARB
    vertex_program_arb = GL_VERTEX_PROGRAM_ARB,
#endif
}; // enum class program_target

} // namespace gl::group
