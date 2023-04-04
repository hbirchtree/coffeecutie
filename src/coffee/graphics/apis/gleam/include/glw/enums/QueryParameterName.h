#pragma once

#include "common.h"

namespace gl::group {

// QueryParameterName
enum class query_parameter_name : u32
{
#ifdef GL_CURRENT_QUERY
    current_query = GL_CURRENT_QUERY,
#endif
#ifdef GL_QUERY_COUNTER_BITS
    query_counter_bits = GL_QUERY_COUNTER_BITS,
#endif
}; // enum class query_parameter_name

} // namespace gl::group
