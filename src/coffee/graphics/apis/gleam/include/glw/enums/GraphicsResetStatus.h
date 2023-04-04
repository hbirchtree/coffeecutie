#pragma once

#include "common.h"

namespace gl::group {

// GraphicsResetStatus
enum class graphics_reset_status : u32
{
#ifdef GL_GUILTY_CONTEXT_RESET
    guilty_context_reset = GL_GUILTY_CONTEXT_RESET,
#endif
#ifdef GL_GUILTY_CONTEXT_RESET_ARB
    guilty_context_reset_arb = GL_GUILTY_CONTEXT_RESET_ARB,
#endif
#ifdef GL_GUILTY_CONTEXT_RESET_EXT
    guilty_context_reset_ext = GL_GUILTY_CONTEXT_RESET_EXT,
#endif
#ifdef GL_GUILTY_CONTEXT_RESET_KHR
    guilty_context_reset_khr = GL_GUILTY_CONTEXT_RESET_KHR,
#endif
#ifdef GL_INNOCENT_CONTEXT_RESET
    innocent_context_reset = GL_INNOCENT_CONTEXT_RESET,
#endif
#ifdef GL_INNOCENT_CONTEXT_RESET_ARB
    innocent_context_reset_arb = GL_INNOCENT_CONTEXT_RESET_ARB,
#endif
#ifdef GL_INNOCENT_CONTEXT_RESET_EXT
    innocent_context_reset_ext = GL_INNOCENT_CONTEXT_RESET_EXT,
#endif
#ifdef GL_INNOCENT_CONTEXT_RESET_KHR
    innocent_context_reset_khr = GL_INNOCENT_CONTEXT_RESET_KHR,
#endif
#ifdef GL_NO_ERROR
    no_error = GL_NO_ERROR,
#endif
#ifdef GL_UNKNOWN_CONTEXT_RESET
    unknown_context_reset = GL_UNKNOWN_CONTEXT_RESET,
#endif
#ifdef GL_UNKNOWN_CONTEXT_RESET_ARB
    unknown_context_reset_arb = GL_UNKNOWN_CONTEXT_RESET_ARB,
#endif
#ifdef GL_UNKNOWN_CONTEXT_RESET_EXT
    unknown_context_reset_ext = GL_UNKNOWN_CONTEXT_RESET_EXT,
#endif
#ifdef GL_UNKNOWN_CONTEXT_RESET_KHR
    unknown_context_reset_khr = GL_UNKNOWN_CONTEXT_RESET_KHR,
#endif
}; // enum class graphics_reset_status

} // namespace gl::group
