#pragma once

#include "common.h"

namespace gl::group {

// ProgramResourceProperty
enum class program_resource_property : u32
{
    active_variables              = 0x9305, // GL_ACTIVE_VARIABLES
    array_size                    = 0x92FB, // GL_ARRAY_SIZE
    array_stride                  = 0x92FE, // GL_ARRAY_STRIDE
    atomic_counter_buffer_index   = 0x9301, // GL_ATOMIC_COUNTER_BUFFER_INDEX
    block_index                   = 0x92FD, // GL_BLOCK_INDEX
    buffer_binding                = 0x9302, // GL_BUFFER_BINDING
    buffer_data_size              = 0x9303, // GL_BUFFER_DATA_SIZE
    compatible_subroutines        = 0x8E4B, // GL_COMPATIBLE_SUBROUTINES
    is_per_patch                  = 0x92E7, // GL_IS_PER_PATCH
    is_row_major                  = 0x9300, // GL_IS_ROW_MAJOR
    location                      = 0x930E, // GL_LOCATION
    location_component            = 0x934A, // GL_LOCATION_COMPONENT
    location_index                = 0x930F, // GL_LOCATION_INDEX
    matrix_stride                 = 0x92FF, // GL_MATRIX_STRIDE
    name_length                   = 0x92F9, // GL_NAME_LENGTH
    num_active_variables          = 0x9304, // GL_NUM_ACTIVE_VARIABLES
    num_compatible_subroutines    = 0x8E4A, // GL_NUM_COMPATIBLE_SUBROUTINES
    offset                        = 0x92FC, // GL_OFFSET
    referenced_by_compute_shader  = 0x930B, // GL_REFERENCED_BY_COMPUTE_SHADER
    referenced_by_fragment_shader = 0x930A, // GL_REFERENCED_BY_FRAGMENT_SHADER
    referenced_by_geometry_shader = 0x9309, // GL_REFERENCED_BY_GEOMETRY_SHADER
    referenced_by_tess_control_shader =
        0x9307, // GL_REFERENCED_BY_TESS_CONTROL_SHADER
    referenced_by_tess_evaluation_shader =
        0x9308, // GL_REFERENCED_BY_TESS_EVALUATION_SHADER
    referenced_by_vertex_shader = 0x9306, // GL_REFERENCED_BY_VERTEX_SHADER
    top_level_array_size        = 0x930C, // GL_TOP_LEVEL_ARRAY_SIZE
    top_level_array_stride      = 0x930D, // GL_TOP_LEVEL_ARRAY_STRIDE
    transform_feedback_buffer_index =
        0x934B, // GL_TRANSFORM_FEEDBACK_BUFFER_INDEX
    transform_feedback_buffer_stride =
        0x934C,       // GL_TRANSFORM_FEEDBACK_BUFFER_STRIDE
    type    = 0x92FA, // GL_TYPE
    uniform = 0x92E1, // GL_UNIFORM
}; // enum class program_resource_property

} // namespace gl::group
