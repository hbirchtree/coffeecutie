#pragma once

#include "common.h"

namespace gl::group {

// QueryCounterTarget
enum class query_counter_target : u32
{
    timestamp     = 0x8E28, // GL_TIMESTAMP
    timestamp_ext = 0x8E28, // GL_TIMESTAMP_EXT
}; // enum class query_counter_target

} // namespace gl::group
