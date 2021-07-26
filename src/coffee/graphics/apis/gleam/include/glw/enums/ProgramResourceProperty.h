#pragma once

#include "common.h"

namespace gl::group {

// ProgramResourceProperty
enum class program_resource_property : ::libc_types::u32
{
#ifdef GL_ACTIVE_VARIABLES
    active_variables = GL_ACTIVE_VARIABLES,
#endif
#ifdef GL_ARRAY_SIZE
    array_size = GL_ARRAY_SIZE,
#endif
#ifdef GL_ARRAY_STRIDE
    array_stride = GL_ARRAY_STRIDE,
#endif
#ifdef GL_ATOMIC_COUNTER_BUFFER_INDEX
    atomic_counter_buffer_index = GL_ATOMIC_COUNTER_BUFFER_INDEX,
#endif
#ifdef GL_BLOCK_INDEX
    block_index = GL_BLOCK_INDEX,
#endif
#ifdef GL_BUFFER_BINDING
    buffer_binding = GL_BUFFER_BINDING,
#endif
#ifdef GL_BUFFER_DATA_SIZE
    buffer_data_size = GL_BUFFER_DATA_SIZE,
#endif
#ifdef GL_COMPATIBLE_SUBROUTINES
    compatible_subroutines = GL_COMPATIBLE_SUBROUTINES,
#endif
#ifdef GL_IS_PER_PATCH
    is_per_patch = GL_IS_PER_PATCH,
#endif
#ifdef GL_IS_ROW_MAJOR
    is_row_major = GL_IS_ROW_MAJOR,
#endif
#ifdef GL_LOCATION
    location = GL_LOCATION,
#endif
#ifdef GL_LOCATION_COMPONENT
    location_component = GL_LOCATION_COMPONENT,
#endif
#ifdef GL_LOCATION_INDEX
    location_index = GL_LOCATION_INDEX,
#endif
#ifdef GL_MATRIX_STRIDE
    matrix_stride = GL_MATRIX_STRIDE,
#endif
#ifdef GL_NAME_LENGTH
    name_length = GL_NAME_LENGTH,
#endif
#ifdef GL_NUM_ACTIVE_VARIABLES
    num_active_variables = GL_NUM_ACTIVE_VARIABLES,
#endif
#ifdef GL_NUM_COMPATIBLE_SUBROUTINES
    num_compatible_subroutines = GL_NUM_COMPATIBLE_SUBROUTINES,
#endif
#ifdef GL_OFFSET
    offset = GL_OFFSET,
#endif
#ifdef GL_REFERENCED_BY_COMPUTE_SHADER
    referenced_by_compute_shader = GL_REFERENCED_BY_COMPUTE_SHADER,
#endif
#ifdef GL_REFERENCED_BY_FRAGMENT_SHADER
    referenced_by_fragment_shader = GL_REFERENCED_BY_FRAGMENT_SHADER,
#endif
#ifdef GL_REFERENCED_BY_GEOMETRY_SHADER
    referenced_by_geometry_shader = GL_REFERENCED_BY_GEOMETRY_SHADER,
#endif
#ifdef GL_REFERENCED_BY_TESS_CONTROL_SHADER
    referenced_by_tess_control_shader = GL_REFERENCED_BY_TESS_CONTROL_SHADER,
#endif
#ifdef GL_REFERENCED_BY_TESS_EVALUATION_SHADER
    referenced_by_tess_evaluation_shader =
        GL_REFERENCED_BY_TESS_EVALUATION_SHADER,
#endif
#ifdef GL_REFERENCED_BY_VERTEX_SHADER
    referenced_by_vertex_shader = GL_REFERENCED_BY_VERTEX_SHADER,
#endif
#ifdef GL_TOP_LEVEL_ARRAY_SIZE
    top_level_array_size = GL_TOP_LEVEL_ARRAY_SIZE,
#endif
#ifdef GL_TOP_LEVEL_ARRAY_STRIDE
    top_level_array_stride = GL_TOP_LEVEL_ARRAY_STRIDE,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER_INDEX
    transform_feedback_buffer_index = GL_TRANSFORM_FEEDBACK_BUFFER_INDEX,
#endif
#ifdef GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE
    transform_feedback_buffer_stride = GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE,
#endif
#ifdef GL_TYPE
    type = GL_TYPE,
#endif
#ifdef GL_UNIFORM
    uniform = GL_UNIFORM,
#endif
}; // enum class program_resource_property

} // namespace gl::group
