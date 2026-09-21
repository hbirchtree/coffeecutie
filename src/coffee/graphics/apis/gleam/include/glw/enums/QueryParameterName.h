#pragma once

#include "common.h"

namespace gl::group {

// QueryParameterName
enum class query_parameter_name : u32
{
    current_query      = 0x8865, // GL_CURRENT_QUERY
    query_counter_bits = 0x8864, // GL_QUERY_COUNTER_BITS
}; // enum class query_parameter_name

} // namespace gl::group
