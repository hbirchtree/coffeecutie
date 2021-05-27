#pragma once

#include "common.h"

namespace gl::groups {

// DebugType
enum class debug_type : ::libc_types::u32 {
#ifdef GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR
    deprecated_behavior = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
#endif
#ifdef GL_DEBUG_TYPE_ERROR
    error = GL_DEBUG_TYPE_ERROR,
#endif
#ifdef GL_DEBUG_TYPE_MARKER
    marker = GL_DEBUG_TYPE_MARKER,
#endif
#ifdef GL_DEBUG_TYPE_OTHER
    other = GL_DEBUG_TYPE_OTHER,
#endif
#ifdef GL_DEBUG_TYPE_PERFORMANCE
    performance = GL_DEBUG_TYPE_PERFORMANCE,
#endif
#ifdef GL_DEBUG_TYPE_POP_GROUP
    pop_group = GL_DEBUG_TYPE_POP_GROUP,
#endif
#ifdef GL_DEBUG_TYPE_PORTABILITY
    portability = GL_DEBUG_TYPE_PORTABILITY,
#endif
#ifdef GL_DEBUG_TYPE_PUSH_GROUP
    push_group = GL_DEBUG_TYPE_PUSH_GROUP,
#endif
#ifdef GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR
    undefined_behavior = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,
#endif
#ifdef GL_DONT_CARE
    dont_care = GL_DONT_CARE,
#endif
}; // enum class debug_type

} // namespace gl::groups
