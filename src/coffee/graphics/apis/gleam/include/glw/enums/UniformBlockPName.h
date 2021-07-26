#pragma once

#include "common.h"

namespace gl::group {

// UniformBlockPName
enum class uniform_block_prop : ::libc_types::u32
{
#ifdef GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS
    uniform_block_active_uniforms = GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,
#endif
#ifdef GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES
    uniform_block_active_uniform_indices =
        GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
#endif
#ifdef GL_UNIFORM_BLOCK_BINDING
    uniform_block_binding = GL_UNIFORM_BLOCK_BINDING,
#endif
#ifdef GL_UNIFORM_BLOCK_DATA_SIZE
    uniform_block_data_size = GL_UNIFORM_BLOCK_DATA_SIZE,
#endif
#ifdef GL_UNIFORM_BLOCK_NAME_LENGTH
    uniform_block_name_length = GL_UNIFORM_BLOCK_NAME_LENGTH,
#endif
#ifdef GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER
    uniform_block_referenced_by_compute_shader =
        GL_UNIFORM_BLOCK_REFERENCED_BY_COMPUTE_SHADER,
#endif
#ifdef GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER
    uniform_block_referenced_by_fragment_shader =
        GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER,
#endif
#ifdef GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER
    uniform_block_referenced_by_geometry_shader =
        GL_UNIFORM_BLOCK_REFERENCED_BY_GEOMETRY_SHADER,
#endif
#ifdef GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER
    uniform_block_referenced_by_tess_control_shader =
        GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_CONTROL_SHADER,
#endif
#ifdef GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER
    uniform_block_referenced_by_tess_evaluation_shader =
        GL_UNIFORM_BLOCK_REFERENCED_BY_TESS_EVALUATION_SHADER,
#endif
#ifdef GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER
    uniform_block_referenced_by_vertex_shader =
        GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER,
#endif
}; // enum class uniform_block_prop

} // namespace gl::group
