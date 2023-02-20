#pragma once

#include "common.h"

namespace gl::group {

// DebugSource
enum class debug_source : ::libc_types::u32
{
#ifdef GL_DEBUG_SOURCE_API
    api = GL_DEBUG_SOURCE_API,
#endif
#ifdef GL_DEBUG_SOURCE_APPLICATION
    application = GL_DEBUG_SOURCE_APPLICATION,
#endif
#ifdef GL_DEBUG_SOURCE_APPLICATION_ARB
    application_arb = GL_DEBUG_SOURCE_APPLICATION_ARB,
#endif
#ifdef GL_DEBUG_SOURCE_APPLICATION_KHR
    application_khr = GL_DEBUG_SOURCE_APPLICATION_KHR,
#endif
#ifdef GL_DEBUG_SOURCE_OTHER
    other = GL_DEBUG_SOURCE_OTHER,
#endif
#ifdef GL_DEBUG_SOURCE_OTHER_ARB
    other_arb = GL_DEBUG_SOURCE_OTHER_ARB,
#endif
#ifdef GL_DEBUG_SOURCE_OTHER_KHR
    other_khr = GL_DEBUG_SOURCE_OTHER_KHR,
#endif
#ifdef GL_DEBUG_SOURCE_SHADER_COMPILER
    shader_compiler = GL_DEBUG_SOURCE_SHADER_COMPILER,
#endif
#ifdef GL_DEBUG_SOURCE_SHADER_COMPILER_ARB
    shader_compiler_arb = GL_DEBUG_SOURCE_SHADER_COMPILER_ARB,
#endif
#ifdef GL_DEBUG_SOURCE_SHADER_COMPILER_KHR
    shader_compiler_khr = GL_DEBUG_SOURCE_SHADER_COMPILER_KHR,
#endif
#ifdef GL_DEBUG_SOURCE_THIRD_PARTY
    third_party = GL_DEBUG_SOURCE_THIRD_PARTY,
#endif
#ifdef GL_DEBUG_SOURCE_THIRD_PARTY_ARB
    third_party_arb = GL_DEBUG_SOURCE_THIRD_PARTY_ARB,
#endif
#ifdef GL_DEBUG_SOURCE_THIRD_PARTY_KHR
    third_party_khr = GL_DEBUG_SOURCE_THIRD_PARTY_KHR,
#endif
#ifdef GL_DEBUG_SOURCE_WINDOW_SYSTEM
    window_system = GL_DEBUG_SOURCE_WINDOW_SYSTEM,
#endif
#ifdef GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB
    window_system_arb = GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB,
#endif
#ifdef GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR
    window_system_khr = GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR,
#endif
#ifdef GL_DONT_CARE
    dont_care = GL_DONT_CARE,
#endif
}; // enum class debug_source

} // namespace gl::group
