#pragma once

#include "common.h"

namespace gl::group {

// UseProgramStageMask
enum class use_program_stage_mask : u32
{
#ifdef GL_ALL_SHADER_BITS
    all_shader_bits = GL_ALL_SHADER_BITS,
#endif
#ifdef GL_ALL_SHADER_BITS_EXT
    all_shader_bits_ext = GL_ALL_SHADER_BITS_EXT,
#endif
#ifdef GL_COMPUTE_SHADER_BIT
    compute_shader_bit = GL_COMPUTE_SHADER_BIT,
#endif
#ifdef GL_FRAGMENT_SHADER_BIT
    fragment_shader_bit = GL_FRAGMENT_SHADER_BIT,
#endif
#ifdef GL_FRAGMENT_SHADER_BIT_EXT
    fragment_shader_bit_ext = GL_FRAGMENT_SHADER_BIT_EXT,
#endif
#ifdef GL_GEOMETRY_SHADER_BIT
    geometry_shader_bit = GL_GEOMETRY_SHADER_BIT,
#endif
#ifdef GL_GEOMETRY_SHADER_BIT_EXT
    geometry_shader_bit_ext = GL_GEOMETRY_SHADER_BIT_EXT,
#endif
#ifdef GL_GEOMETRY_SHADER_BIT_OES
    geometry_shader_bit_oes = GL_GEOMETRY_SHADER_BIT_OES,
#endif
#ifdef GL_MESH_SHADER_BIT_NV
    mesh_shader_bit_nv = GL_MESH_SHADER_BIT_NV,
#endif
#ifdef GL_TASK_SHADER_BIT_NV
    task_shader_bit_nv = GL_TASK_SHADER_BIT_NV,
#endif
#ifdef GL_TESS_CONTROL_SHADER_BIT
    tess_control_shader_bit = GL_TESS_CONTROL_SHADER_BIT,
#endif
#ifdef GL_TESS_CONTROL_SHADER_BIT_EXT
    tess_control_shader_bit_ext = GL_TESS_CONTROL_SHADER_BIT_EXT,
#endif
#ifdef GL_TESS_CONTROL_SHADER_BIT_OES
    tess_control_shader_bit_oes = GL_TESS_CONTROL_SHADER_BIT_OES,
#endif
#ifdef GL_TESS_EVALUATION_SHADER_BIT
    tess_evaluation_shader_bit = GL_TESS_EVALUATION_SHADER_BIT,
#endif
#ifdef GL_TESS_EVALUATION_SHADER_BIT_EXT
    tess_evaluation_shader_bit_ext = GL_TESS_EVALUATION_SHADER_BIT_EXT,
#endif
#ifdef GL_TESS_EVALUATION_SHADER_BIT_OES
    tess_evaluation_shader_bit_oes = GL_TESS_EVALUATION_SHADER_BIT_OES,
#endif
#ifdef GL_VERTEX_SHADER_BIT
    vertex_shader_bit = GL_VERTEX_SHADER_BIT,
#endif
#ifdef GL_VERTEX_SHADER_BIT_EXT
    vertex_shader_bit_ext = GL_VERTEX_SHADER_BIT_EXT,
#endif
}; // enum class use_program_stage_mask
C_FLAGS(use_program_stage_mask, u32);

} // namespace gl::group
