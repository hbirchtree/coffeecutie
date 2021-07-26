#pragma once

#ifdef GL_NV_fbo_color_attachments
#include "../enums/DrawBufferModeATI.h"
#include "../enums/InvalidateFramebufferAttachment.h"
namespace gl::nv::fbo_color_attachments {
using gl::group::draw_buffer_mode_ati;
using gl::group::invalidate_framebuffer_attachment;
namespace values {
constexpr libc_types::u32 max_color_attachments = 0x8CDF;
} // namespace values
} // namespace gl::nv::fbo_color_attachments
#endif // GL_NV_fbo_color_attachments
namespace gl::nv::fbo_color_attachments {
constexpr auto name = "GL_NV_fbo_color_attachments";
}
