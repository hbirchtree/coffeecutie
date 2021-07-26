#pragma once

#include "common.h"

namespace gl::group {

// GraphicsResetStatus
enum class graphics_reset_status : ::libc_types::u32
{
#ifdef GL_GUILTY_CONTEXT_RESET
    guilty_context_reset = GL_GUILTY_CONTEXT_RESET,
#endif
#ifdef GL_INNOCENT_CONTEXT_RESET
    innocent_context_reset = GL_INNOCENT_CONTEXT_RESET,
#endif
#ifdef GL_NO_ERROR
    no_error = GL_NO_ERROR,
#endif
#ifdef GL_UNKNOWN_CONTEXT_RESET
    unknown_context_reset = GL_UNKNOWN_CONTEXT_RESET,
#endif
}; // enum class graphics_reset_status

} // namespace gl::group
