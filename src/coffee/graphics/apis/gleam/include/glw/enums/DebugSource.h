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
#ifdef GL_DEBUG_SOURCE_OTHER
    other = GL_DEBUG_SOURCE_OTHER,
#endif
#ifdef GL_DEBUG_SOURCE_SHADER_COMPILER
    shader_compiler = GL_DEBUG_SOURCE_SHADER_COMPILER,
#endif
#ifdef GL_DEBUG_SOURCE_THIRD_PARTY
    third_party = GL_DEBUG_SOURCE_THIRD_PARTY,
#endif
#ifdef GL_DEBUG_SOURCE_WINDOW_SYSTEM
    window_system = GL_DEBUG_SOURCE_WINDOW_SYSTEM,
#endif
#ifdef GL_DONT_CARE
    dont_care = GL_DONT_CARE,
#endif
}; // enum class debug_source

} // namespace gl::group
