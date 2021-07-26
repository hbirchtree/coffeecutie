#pragma once

#include "common.h"

namespace gl::group {

// ShaderParameterName
enum class shader_parameter_name : ::libc_types::u32
{
#ifdef GL_COMPILE_STATUS
    compile_status = GL_COMPILE_STATUS,
#endif
#ifdef GL_DELETE_STATUS
    delete_status = GL_DELETE_STATUS,
#endif
#ifdef GL_INFO_LOG_LENGTH
    info_log_length = GL_INFO_LOG_LENGTH,
#endif
#ifdef GL_SHADER_SOURCE_LENGTH
    shader_source_length = GL_SHADER_SOURCE_LENGTH,
#endif
#ifdef GL_SHADER_TYPE
    shader_type = GL_SHADER_TYPE,
#endif
}; // enum class shader_parameter_name

} // namespace gl::group
