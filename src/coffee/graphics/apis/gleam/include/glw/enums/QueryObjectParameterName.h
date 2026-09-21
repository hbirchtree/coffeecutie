#pragma once

#include "common.h"

namespace gl::group {

// QueryObjectParameterName
enum class query_object_parameter_name : u32
{
    query_result           = 0x8866, // GL_QUERY_RESULT
    query_result_available = 0x8867, // GL_QUERY_RESULT_AVAILABLE
    query_result_no_wait   = 0x9194, // GL_QUERY_RESULT_NO_WAIT
    query_target           = 0x82EA, // GL_QUERY_TARGET
}; // enum class query_object_parameter_name

} // namespace gl::group
