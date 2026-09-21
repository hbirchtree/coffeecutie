#pragma once

#include "common.h"

namespace gl::group {

// UniformBlockPName
enum class uniform_block_prop : u32
{
    uniform_block_active_uniforms = 0x8A42, // GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS
    uniform_block_active_uniform_indices =
        0x8A43, // GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES
    uniform_block_binding     = 0x8A3F, // GL_UNIFORM_BLOCK_BINDING
    uniform_block_data_size   = 0x8A40, // GL_UNIFORM_BLOCK_DATA_SIZE
    uniform_block_name_length = 0x8A41, // GL_UNIFORM_BLOCK_NAME_LENGTH
    uniform_block_referenced_by_compute_shader =
        0x90EC, // GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER
    uniform_block_referenced_by_fragment_shader =
        0x8A46, // GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER
    uniform_block_referenced_by_geometry_shader =
        0x8A45, // GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER
    uniform_block_referenced_by_tess_control_shader =
        0x84F0, // GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER
    uniform_block_referenced_by_tess_evaluation_shader =
        0x84F1, // GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER
    uniform_block_referenced_by_vertex_shader =
        0x8A44, // GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER
}; // enum class uniform_block_prop

} // namespace gl::group
