#pragma once

#include "common.h"

namespace gl::group {

// QueryCounterTarget
enum class query_counter_target : u32
{
#ifdef GL_TIMESTAMP
    timestamp = GL_TIMESTAMP,
#endif
#ifdef GL_TIMESTAMP_EXT
    timestamp_ext = GL_TIMESTAMP_EXT,
#endif
}; // enum class query_counter_target

} // namespace gl::group
