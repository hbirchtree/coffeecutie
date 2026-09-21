#pragma once

#include "common.h"

namespace gl::group {

// UseProgramStageMask
enum class use_program_stage_mask : u32
{
    all_shader_bits             = 0xFFFFFFFF, // GL_ALL_SHADER_BITS
    compute_shader_bit          = 0x00000020, // GL_COMPUTE_SHADER_BIT
    fragment_shader_bit         = 0x00000002, // GL_FRAGMENT_SHADER_BIT
    geometry_shader_bit         = 0x00000004, // GL_GEOMETRY_SHADER_BIT
    tess_control_shader_bit     = 0x00000008, // GL_TESS_CONTROL_SHADER_BIT
    tess_evaluation_shader_bit  = 0x00000010, // GL_TESS_EVALUATION_SHADER_BIT
    vertex_shader_bit           = 0x00000001, // GL_VERTEX_SHADER_BIT
    all_shader_bits_ext         = 0xFFFFFFFF, // GL_ALL_SHADER_BITS_EXT
    fragment_shader_bit_ext     = 0x00000002, // GL_FRAGMENT_SHADER_BIT_EXT
    geometry_shader_bit_ext     = 0x00000004, // GL_GEOMETRY_SHADER_BIT_EXT
    geometry_shader_bit_oes     = 0x00000004, // GL_GEOMETRY_SHADER_BIT_OES
    mesh_shader_bit_ext         = 0x00000040, // GL_MESH_SHADER_BIT_EXT
    mesh_shader_bit_nv          = 0x00000040, // GL_MESH_SHADER_BIT_NV
    task_shader_bit_ext         = 0x00000080, // GL_TASK_SHADER_BIT_EXT
    task_shader_bit_nv          = 0x00000080, // GL_TASK_SHADER_BIT_NV
    tess_control_shader_bit_ext = 0x00000008, // GL_TESS_CONTROL_SHADER_BIT_EXT
    tess_control_shader_bit_oes = 0x00000008, // GL_TESS_CONTROL_SHADER_BIT_OES
    tess_evaluation_shader_bit_ext =
        0x00000010, // GL_TESS_EVALUATION_SHADER_BIT_EXT
    tess_evaluation_shader_bit_oes =
        0x00000010,                     // GL_TESS_EVALUATION_SHADER_BIT_OES
    vertex_shader_bit_ext = 0x00000001, // GL_VERTEX_SHADER_BIT_EXT
}; // enum class use_program_stage_mask
C_FLAGS(use_program_stage_mask, u32);

} // namespace gl::group
