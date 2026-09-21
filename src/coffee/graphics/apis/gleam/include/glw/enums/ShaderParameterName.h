#pragma once

#include "common.h"

namespace gl::group {

// ShaderParameterName
enum class shader_parameter_name : u32
{
    compile_status        = 0x8B81, // GL_COMPILE_STATUS
    delete_status         = 0x8B80, // GL_DELETE_STATUS
    info_log_length       = 0x8B84, // GL_INFO_LOG_LENGTH
    shader_source_length  = 0x8B88, // GL_SHADER_SOURCE_LENGTH
    shader_type           = 0x8B4F, // GL_SHADER_TYPE
    completion_status_khr = 0x91B1, // GL_COMPLETION_STATUS_KHR
}; // enum class shader_parameter_name

} // namespace gl::group
