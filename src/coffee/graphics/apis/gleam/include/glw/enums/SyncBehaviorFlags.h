#pragma once

#include "common.h"

namespace gl::groups {

// SyncBehaviorFlags
enum class sync_behavior_flags : ::libc_types::u32 {
#ifdef GL_NONE
    none = GL_NONE,
#endif
}; // enum class sync_behavior_flags

} // namespace gl::groups
