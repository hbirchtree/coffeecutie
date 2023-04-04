#pragma once

#include "common.h"

namespace gl::group {

// QueryObjectParameterName
enum class query_object_parameter_name : u32
{
#ifdef GL_QUERY_RESULT
    query_result = GL_QUERY_RESULT,
#endif
#ifdef GL_QUERY_RESULT_AVAILABLE
    query_result_available = GL_QUERY_RESULT_AVAILABLE,
#endif
#ifdef GL_QUERY_RESULT_NO_WAIT
    query_result_no_wait = GL_QUERY_RESULT_NO_WAIT,
#endif
#ifdef GL_QUERY_TARGET
    query_target = GL_QUERY_TARGET,
#endif
}; // enum class query_object_parameter_name

} // namespace gl::group
