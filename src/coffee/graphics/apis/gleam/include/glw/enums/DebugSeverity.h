#pragma once

#include "common.h"

namespace gl::group {

// DebugSeverity
enum class debug_severity : ::libc_types::u32
{
#ifdef GL_DEBUG_SEVERITY_HIGH
    high = GL_DEBUG_SEVERITY_HIGH,
#endif
#ifdef GL_DEBUG_SEVERITY_HIGH_AMD
    high_amd = GL_DEBUG_SEVERITY_HIGH_AMD,
#endif
#ifdef GL_DEBUG_SEVERITY_HIGH_ARB
    high_arb = GL_DEBUG_SEVERITY_HIGH_ARB,
#endif
#ifdef GL_DEBUG_SEVERITY_HIGH_KHR
    high_khr = GL_DEBUG_SEVERITY_HIGH_KHR,
#endif
#ifdef GL_DEBUG_SEVERITY_LOW
    low = GL_DEBUG_SEVERITY_LOW,
#endif
#ifdef GL_DEBUG_SEVERITY_LOW_AMD
    low_amd = GL_DEBUG_SEVERITY_LOW_AMD,
#endif
#ifdef GL_DEBUG_SEVERITY_LOW_ARB
    low_arb = GL_DEBUG_SEVERITY_LOW_ARB,
#endif
#ifdef GL_DEBUG_SEVERITY_LOW_KHR
    low_khr = GL_DEBUG_SEVERITY_LOW_KHR,
#endif
#ifdef GL_DEBUG_SEVERITY_MEDIUM
    medium = GL_DEBUG_SEVERITY_MEDIUM,
#endif
#ifdef GL_DEBUG_SEVERITY_MEDIUM_AMD
    medium_amd = GL_DEBUG_SEVERITY_MEDIUM_AMD,
#endif
#ifdef GL_DEBUG_SEVERITY_MEDIUM_ARB
    medium_arb = GL_DEBUG_SEVERITY_MEDIUM_ARB,
#endif
#ifdef GL_DEBUG_SEVERITY_MEDIUM_KHR
    medium_khr = GL_DEBUG_SEVERITY_MEDIUM_KHR,
#endif
#ifdef GL_DEBUG_SEVERITY_NOTIFICATION
    notification = GL_DEBUG_SEVERITY_NOTIFICATION,
#endif
#ifdef GL_DEBUG_SEVERITY_NOTIFICATION_KHR
    notification_khr = GL_DEBUG_SEVERITY_NOTIFICATION_KHR,
#endif
#ifdef GL_DONT_CARE
    dont_care = GL_DONT_CARE,
#endif
}; // enum class debug_severity

} // namespace gl::group
