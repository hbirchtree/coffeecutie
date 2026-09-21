#pragma once

#include "common.h"

namespace gl::group {

// ProgramPropertyARB
enum class program_property_arb : u32
{
    active_atomic_counter_buffers = 0x92D9, // GL_ACTIVE_ATOMIC_COUNTER_BUFFERS
    active_attributes             = 0x8B89, // GL_ACTIVE_ATTRIBUTES
    active_attribute_max_length   = 0x8B8A, // GL_ACTIVE_ATTRIBUTE_MAX_LENGTH
    active_uniforms               = 0x8B86, // GL_ACTIVE_UNIFORMS
    active_uniform_blocks         = 0x8A36, // GL_ACTIVE_UNIFORM_BLOCKS
    active_uniform_block_max_name_length =
        0x8A35, // GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH
    active_uniform_max_length = 0x8B87, // GL_ACTIVE_UNIFORM_MAX_LENGTH
    attached_shaders          = 0x8B85, // GL_ATTACHED_SHADERS
    compute_work_group_size   = 0x8267, // GL_COMPUTE_WORK_GROUP_SIZE
    delete_status             = 0x8B80, // GL_DELETE_STATUS
    geometry_input_type       = 0x8917, // GL_GEOMETRY_INPUT_TYPE
    geometry_output_type      = 0x8918, // GL_GEOMETRY_OUTPUT_TYPE
    geometry_vertices_out     = 0x8916, // GL_GEOMETRY_VERTICES_OUT
    info_log_length           = 0x8B84, // GL_INFO_LOG_LENGTH
    link_status               = 0x8B82, // GL_LINK_STATUS
    program_binary_length     = 0x8741, // GL_PROGRAM_BINARY_LENGTH
    transform_feedback_buffer_mode =
        0x8C7F,                           // GL_TRANSFORM_FEEDBACK_BUFFER_MODE
    transform_feedback_varyings = 0x8C83, // GL_TRANSFORM_FEEDBACK_VARYINGS
    transform_feedback_varying_max_length =
        0x8C76,                     // GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH
    validate_status       = 0x8B83, // GL_VALIDATE_STATUS
    completion_status_arb = 0x91B1, // GL_COMPLETION_STATUS_ARB
    completion_status_khr = 0x91B1, // GL_COMPLETION_STATUS_KHR
}; // enum class program_property_arb

} // namespace gl::group
