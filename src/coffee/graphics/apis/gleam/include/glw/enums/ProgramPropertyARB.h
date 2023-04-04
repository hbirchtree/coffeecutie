#pragma once

#include "common.h"

namespace gl::group {

// ProgramPropertyARB
enum class program_property_arb : u32
{
#ifdef GL_ACTIVE_ATOMIC_COUNTER_BUFFERS
    active_atomic_counter_buffers = GL_ACTIVE_ATOMIC_COUNTER_BUFFERS,
#endif
#ifdef GL_ACTIVE_ATTRIBUTES
    active_attributes = GL_ACTIVE_ATTRIBUTES,
#endif
#ifdef GL_ACTIVE_ATTRIBUTE_MAX_LENGTH
    active_attribute_max_length = GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
#endif
#ifdef GL_ACTIVE_UNIFORMS
    active_uniforms = GL_ACTIVE_UNIFORMS,
#endif
#ifdef GL_ACTIVE_UNIFORM_BLOCKS
    active_uniform_blocks = GL_ACTIVE_UNIFORM_BLOCKS,
#endif
#ifdef GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH
    active_uniform_block_max_name_length =
        GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH,
#endif
#ifdef GL_ACTIVE_UNIFORM_MAX_LENGTH
    active_uniform_max_length = GL_ACTIVE_UNIFORM_MAX_LENGTH,
#endif
#ifdef GL_ATTACHED_SHADERS
    attached_shaders = GL_ATTACHED_SHADERS,
#endif
#ifdef GL_COMPUTE_WORK_GROUP_SIZE
    compute_work_group_size = GL_COMPUTE_WORK_GROUP_SIZE,
#endif
#ifdef GL_DELETE_STATUS
    delete_status = GL_DELETE_STATUS,
#endif
#ifdef GL_GEOMETRY_INPUT_TYPE
    geometry_input_type = GL_GEOMETRY_INPUT_TYPE,
#endif
#ifdef GL_GEOMETRY_OUTPUT_TYPE
    geometry_output_type = GL_GEOMETRY_OUTPUT_TYPE,
#endif
#ifdef GL_GEOMETRY_VERTICES_OUT
    geometry_vertices_out = GL_GEOMETRY_VERTICES_OUT,
#endif
#ifdef GL_INFO_LOG_LENGTH
    info_log_length = GL_INFO_LOG_LENGTH,
#endif
#ifdef GL_LINK_STATUS
    link_status = GL_LINK_STATUS,
#endif
#ifdef GL_PROGRAM_BINARY_LENGTH
    program_binary_length = GL_PROGRAM_BINARY_LENGTH,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER_MODE
    transform_feedback_buffer_mode = GL_TRANSFORM_FEEDBACK_BUFFER_MODE,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_VARYINGS
    transform_feedback_varyings = GL_TRANSFORM_FEEDBACK_VARYINGS,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH
    transform_feedback_varying_max_length =
        GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH,
#endif
#ifdef GL_VALIDATE_STATUS
    validate_status = GL_VALIDATE_STATUS,
#endif
}; // enum class program_property_arb

} // namespace gl::group
