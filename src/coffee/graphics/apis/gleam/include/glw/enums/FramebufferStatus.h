#pragma once

#include "common.h"

namespace gl::group {

// FramebufferStatus
enum class framebuffer_status : u32
{
    framebuffer_complete = 0x8CD5, // GL_FRAMEBUFFER_COMPLETE
    framebuffer_incomplete_attachment =
        0x8CD6, // GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
    framebuffer_incomplete_draw_buffer =
        0x8CDB, // GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
    framebuffer_incomplete_layer_targets =
        0x8DA8, // GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
    framebuffer_incomplete_missing_attachment =
        0x8CD7, // GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
    framebuffer_incomplete_multisample =
        0x8D56, // GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
    framebuffer_incomplete_read_buffer =
        0x8CDC,                       // GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
    framebuffer_undefined   = 0x8219, // GL_FRAMEBUFFER_UNDEFINED
    framebuffer_unsupported = 0x8CDD, // GL_FRAMEBUFFER_UNSUPPORTED
}; // enum class framebuffer_status

} // namespace gl::group
