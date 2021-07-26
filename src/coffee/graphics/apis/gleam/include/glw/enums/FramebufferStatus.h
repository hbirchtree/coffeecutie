#pragma once

#include "common.h"

namespace gl::group {

// FramebufferStatus
enum class framebuffer_status : ::libc_types::u32
{
#ifdef GL_FRAMEBUFFER_COMPLETE
    framebuffer_complete = GL_FRAMEBUFFER_COMPLETE,
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
    framebuffer_incomplete_attachment = GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
    framebuffer_incomplete_draw_buffer = GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER,
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
    framebuffer_incomplete_layer_targets =
        GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS,
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
    framebuffer_incomplete_missing_attachment =
        GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
    framebuffer_incomplete_multisample = GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE,
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
    framebuffer_incomplete_read_buffer = GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER,
#endif
#ifdef GL_FRAMEBUFFER_UNDEFINED
    framebuffer_undefined = GL_FRAMEBUFFER_UNDEFINED,
#endif
#ifdef GL_FRAMEBUFFER_UNSUPPORTED
    framebuffer_unsupported = GL_FRAMEBUFFER_UNSUPPORTED,
#endif
}; // enum class framebuffer_status

} // namespace gl::group
