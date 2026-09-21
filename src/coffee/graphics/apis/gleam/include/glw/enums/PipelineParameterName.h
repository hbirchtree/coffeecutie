#pragma once

#include "common.h"

namespace gl::group {

// PipelineParameterName
enum class pipeline_parameter_name : u32
{
    active_program         = 0x8259, // GL_ACTIVE_PROGRAM
    fragment_shader        = 0x8B30, // GL_FRAGMENT_SHADER
    geometry_shader        = 0x8DD9, // GL_GEOMETRY_SHADER
    info_log_length        = 0x8B84, // GL_INFO_LOG_LENGTH
    tess_control_shader    = 0x8E88, // GL_TESS_CONTROL_SHADER
    tess_evaluation_shader = 0x8E87, // GL_TESS_EVALUATION_SHADER
    vertex_shader          = 0x8B31, // GL_VERTEX_SHADER
}; // enum class pipeline_parameter_name

} // namespace gl::group
