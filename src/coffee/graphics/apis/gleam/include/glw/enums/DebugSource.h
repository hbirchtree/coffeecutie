#pragma once

#include "common.h"

namespace gl::group {

// DebugSource
enum class debug_source : u32
{
    api                 = 0x8246, // GL_DEBUG_SOURCE_API
    application         = 0x824A, // GL_DEBUG_SOURCE_APPLICATION
    other               = 0x824B, // GL_DEBUG_SOURCE_OTHER
    shader_compiler     = 0x8248, // GL_DEBUG_SOURCE_SHADER_COMPILER
    third_party         = 0x8249, // GL_DEBUG_SOURCE_THIRD_PARTY
    window_system       = 0x8247, // GL_DEBUG_SOURCE_WINDOW_SYSTEM
    dont_care           = 0x1100, // GL_DONT_CARE
    application_arb     = 0x824A, // GL_DEBUG_SOURCE_APPLICATION_ARB
    application_khr     = 0x824A, // GL_DEBUG_SOURCE_APPLICATION_KHR
    other_arb           = 0x824B, // GL_DEBUG_SOURCE_OTHER_ARB
    other_khr           = 0x824B, // GL_DEBUG_SOURCE_OTHER_KHR
    shader_compiler_arb = 0x8248, // GL_DEBUG_SOURCE_SHADER_COMPILER_ARB
    shader_compiler_khr = 0x8248, // GL_DEBUG_SOURCE_SHADER_COMPILER_KHR
    third_party_arb     = 0x8249, // GL_DEBUG_SOURCE_THIRD_PARTY_ARB
    third_party_khr     = 0x8249, // GL_DEBUG_SOURCE_THIRD_PARTY_KHR
    window_system_arb   = 0x8247, // GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB
    window_system_khr   = 0x8247, // GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR
}; // enum class debug_source

} // namespace gl::group
