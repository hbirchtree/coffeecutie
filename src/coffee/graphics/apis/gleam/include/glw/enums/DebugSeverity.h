#pragma once

#include "common.h"

namespace gl::group {

// DebugSeverity
enum class debug_severity : u32
{
    high             = 0x9146, // GL_DEBUG_SEVERITY_HIGH
    low              = 0x9148, // GL_DEBUG_SEVERITY_LOW
    medium           = 0x9147, // GL_DEBUG_SEVERITY_MEDIUM
    notification     = 0x826B, // GL_DEBUG_SEVERITY_NOTIFICATION
    dont_care        = 0x1100, // GL_DONT_CARE
    high_amd         = 0x9146, // GL_DEBUG_SEVERITY_HIGH_AMD
    high_arb         = 0x9146, // GL_DEBUG_SEVERITY_HIGH_ARB
    high_khr         = 0x9146, // GL_DEBUG_SEVERITY_HIGH_KHR
    low_amd          = 0x9148, // GL_DEBUG_SEVERITY_LOW_AMD
    low_arb          = 0x9148, // GL_DEBUG_SEVERITY_LOW_ARB
    low_khr          = 0x9148, // GL_DEBUG_SEVERITY_LOW_KHR
    medium_amd       = 0x9147, // GL_DEBUG_SEVERITY_MEDIUM_AMD
    medium_arb       = 0x9147, // GL_DEBUG_SEVERITY_MEDIUM_ARB
    medium_khr       = 0x9147, // GL_DEBUG_SEVERITY_MEDIUM_KHR
    notification_khr = 0x826B, // GL_DEBUG_SEVERITY_NOTIFICATION_KHR
}; // enum class debug_severity

} // namespace gl::group
