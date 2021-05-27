#pragma once

#include "common.h"

namespace gl::groups {

// PipelineParameterName
enum class pipeline_parameter_name : ::libc_types::u32 {
#ifdef GL_ACTIVE_PROGRAM
    active_program = GL_ACTIVE_PROGRAM,
#endif
#ifdef GL_FRAGMENT_SHADER
    fragment_shader = GL_FRAGMENT_SHADER,
#endif
#ifdef GL_GEOMETRY_SHADER
    geometry_shader = GL_GEOMETRY_SHADER,
#endif
#ifdef GL_INFO_LOG_LENGTH
    info_log_length = GL_INFO_LOG_LENGTH,
#endif
#ifdef GL_TESS_CONTROL_SHADER
    tess_control_shader = GL_TESS_CONTROL_SHADER,
#endif
#ifdef GL_TESS_EVALUATION_SHADER
    tess_evaluation_shader = GL_TESS_EVALUATION_SHADER,
#endif
#ifdef GL_VERTEX_SHADER
    vertex_shader = GL_VERTEX_SHADER,
#endif
}; // enum class pipeline_parameter_name

} // namespace gl::groups
