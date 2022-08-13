#pragma once

#include "common.h"

namespace gl::group {

// QueryCounterTarget
enum class query_counter_target : ::libc_types::u32
{
#ifdef GL_TIMESTAMP
    timestamp = GL_TIMESTAMP,
#endif
}; // enum class query_counter_target

} // namespace gl::group