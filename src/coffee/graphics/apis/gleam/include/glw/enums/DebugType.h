#pragma once

#include "common.h"

namespace gl::group {

// DebugType
enum class debug_type : u32
{
#ifdef GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR
    deprecated_behavior = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
#endif
#ifdef GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB
    deprecated_behavior_arb = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB,
#endif
#ifdef GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR
    deprecated_behavior_khr = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR,
#endif
#ifdef GL_DEBUG_TYPE_ERROR
    error = GL_DEBUG_TYPE_ERROR,
#endif
#ifdef GL_DEBUG_TYPE_ERROR_ARB
    error_arb = GL_DEBUG_TYPE_ERROR_ARB,
#endif
#ifdef GL_DEBUG_TYPE_ERROR_KHR
    error_khr = GL_DEBUG_TYPE_ERROR_KHR,
#endif
#ifdef GL_DEBUG_TYPE_MARKER
    marker = GL_DEBUG_TYPE_MARKER,
#endif
#ifdef GL_DEBUG_TYPE_MARKER_KHR
    marker_khr = GL_DEBUG_TYPE_MARKER_KHR,
#endif
#ifdef GL_DEBUG_TYPE_OTHER
    other = GL_DEBUG_TYPE_OTHER,
#endif
#ifdef GL_DEBUG_TYPE_OTHER_ARB
    other_arb = GL_DEBUG_TYPE_OTHER_ARB,
#endif
#ifdef GL_DEBUG_TYPE_OTHER_KHR
    other_khr = GL_DEBUG_TYPE_OTHER_KHR,
#endif
#ifdef GL_DEBUG_TYPE_PERFORMANCE
    performance = GL_DEBUG_TYPE_PERFORMANCE,
#endif
#ifdef GL_DEBUG_TYPE_PERFORMANCE_ARB
    performance_arb = GL_DEBUG_TYPE_PERFORMANCE_ARB,
#endif
#ifdef GL_DEBUG_TYPE_PERFORMANCE_KHR
    performance_khr = GL_DEBUG_TYPE_PERFORMANCE_KHR,
#endif
#ifdef GL_DEBUG_TYPE_POP_GROUP
    pop_group = GL_DEBUG_TYPE_POP_GROUP,
#endif
#ifdef GL_DEBUG_TYPE_POP_GROUP_KHR
    pop_group_khr = GL_DEBUG_TYPE_POP_GROUP_KHR,
#endif
#ifdef GL_DEBUG_TYPE_PORTABILITY
    portability = GL_DEBUG_TYPE_PORTABILITY,
#endif
#ifdef GL_DEBUG_TYPE_PORTABILITY_ARB
    portability_arb = GL_DEBUG_TYPE_PORTABILITY_ARB,
#endif
#ifdef GL_DEBUG_TYPE_PORTABILITY_KHR
    portability_khr = GL_DEBUG_TYPE_PORTABILITY_KHR,
#endif
#ifdef GL_DEBUG_TYPE_PUSH_GROUP
    push_group = GL_DEBUG_TYPE_PUSH_GROUP,
#endif
#ifdef GL_DEBUG_TYPE_PUSH_GROUP_KHR
    push_group_khr = GL_DEBUG_TYPE_PUSH_GROUP_KHR,
#endif
#ifdef GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR
    undefined_behavior = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,
#endif
#ifdef GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB
    undefined_behavior_arb = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB,
#endif
#ifdef GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR
    undefined_behavior_khr = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR,
#endif
#ifdef GL_DONT_CARE
    dont_care = GL_DONT_CARE,
#endif
}; // enum class debug_type

} // namespace gl::group
