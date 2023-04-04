#pragma once

#include "common.h"

namespace gl::group {

// SyncBehaviorFlags
enum class sync_behavior_flags : u32
{
#ifdef GL_NONE
    none = GL_NONE,
#endif
}; // enum class sync_behavior_flags

} // namespace gl::group
