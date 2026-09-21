#pragma once

#include "common.h"

namespace gl::group {

// DebugType
enum class debug_type : u32
{
    deprecated_behavior     = 0x824D, // GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR
    error                   = 0x824C, // GL_DEBUG_TYPE_ERROR
    marker                  = 0x8268, // GL_DEBUG_TYPE_MARKER
    other                   = 0x8251, // GL_DEBUG_TYPE_OTHER
    performance             = 0x8250, // GL_DEBUG_TYPE_PERFORMANCE
    pop_group               = 0x826A, // GL_DEBUG_TYPE_POP_GROUP
    portability             = 0x824F, // GL_DEBUG_TYPE_PORTABILITY
    push_group              = 0x8269, // GL_DEBUG_TYPE_PUSH_GROUP
    undefined_behavior      = 0x824E, // GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR
    dont_care               = 0x1100, // GL_DONT_CARE
    deprecated_behavior_arb = 0x824D, // GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB
    deprecated_behavior_khr = 0x824D, // GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR
    error_arb               = 0x824C, // GL_DEBUG_TYPE_ERROR_ARB
    error_khr               = 0x824C, // GL_DEBUG_TYPE_ERROR_KHR
    marker_khr              = 0x8268, // GL_DEBUG_TYPE_MARKER_KHR
    other_arb               = 0x8251, // GL_DEBUG_TYPE_OTHER_ARB
    other_khr               = 0x8251, // GL_DEBUG_TYPE_OTHER_KHR
    performance_arb         = 0x8250, // GL_DEBUG_TYPE_PERFORMANCE_ARB
    performance_khr         = 0x8250, // GL_DEBUG_TYPE_PERFORMANCE_KHR
    pop_group_khr           = 0x826A, // GL_DEBUG_TYPE_POP_GROUP_KHR
    portability_arb         = 0x824F, // GL_DEBUG_TYPE_PORTABILITY_ARB
    portability_khr         = 0x824F, // GL_DEBUG_TYPE_PORTABILITY_KHR
    push_group_khr          = 0x8269, // GL_DEBUG_TYPE_PUSH_GROUP_KHR
    undefined_behavior_arb  = 0x824E, // GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB
    undefined_behavior_khr  = 0x824E, // GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR
}; // enum class debug_type

} // namespace gl::group
