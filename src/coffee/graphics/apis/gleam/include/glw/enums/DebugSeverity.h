#pragma once

#include "common.h"

namespace gl::groups {

// DebugSeverity
enum class debug_severity : ::libc_types::u32 {
#ifdef GL_DEBUG_SEVERITY_HIGH
    high = GL_DEBUG_SEVERITY_HIGH,
#endif
#ifdef GL_DEBUG_SEVERITY_LOW
    low = GL_DEBUG_SEVERITY_LOW,
#endif
#ifdef GL_DEBUG_SEVERITY_MEDIUM
    medium = GL_DEBUG_SEVERITY_MEDIUM,
#endif
#ifdef GL_DEBUG_SEVERITY_NOTIFICATION
    notification = GL_DEBUG_SEVERITY_NOTIFICATION,
#endif
#ifdef GL_DONT_CARE
    dont_care = GL_DONT_CARE,
#endif
}; // enum class debug_severity

} // namespace gl::groups
