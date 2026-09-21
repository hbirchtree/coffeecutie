#pragma once

#include "common.h"

namespace gl::group {

// GraphicsResetStatus
enum class graphics_reset_status : u32
{
    guilty_context_reset       = 0x8253, // GL_GUILTY_CONTEXT_RESET
    innocent_context_reset     = 0x8254, // GL_INNOCENT_CONTEXT_RESET
    no_error                   = 0,      // GL_NO_ERROR
    unknown_context_reset      = 0x8255, // GL_UNKNOWN_CONTEXT_RESET
    guilty_context_reset_arb   = 0x8253, // GL_GUILTY_CONTEXT_RESET_ARB
    guilty_context_reset_ext   = 0x8253, // GL_GUILTY_CONTEXT_RESET_EXT
    guilty_context_reset_khr   = 0x8253, // GL_GUILTY_CONTEXT_RESET_KHR
    innocent_context_reset_arb = 0x8254, // GL_INNOCENT_CONTEXT_RESET_ARB
    innocent_context_reset_ext = 0x8254, // GL_INNOCENT_CONTEXT_RESET_EXT
    innocent_context_reset_khr = 0x8254, // GL_INNOCENT_CONTEXT_RESET_KHR
    unknown_context_reset_arb  = 0x8255, // GL_UNKNOWN_CONTEXT_RESET_ARB
    unknown_context_reset_ext  = 0x8255, // GL_UNKNOWN_CONTEXT_RESET_EXT
    unknown_context_reset_khr  = 0x8255, // GL_UNKNOWN_CONTEXT_RESET_KHR
}; // enum class graphics_reset_status

} // namespace gl::group
