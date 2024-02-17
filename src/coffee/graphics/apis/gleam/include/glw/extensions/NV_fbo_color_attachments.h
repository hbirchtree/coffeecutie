#pragma once

#ifdef GL_NV_fbo_color_attachments
#include "../enums/DrawBufferMode.h"
#include "../enums/GetPName.h"
#include "../enums/InvalidateFramebufferAttachment.h"

namespace gl::nv::fbo_color_attachments {
using gl::group::draw_buffer_mode;
using gl::group::get_prop;
using gl::group::invalidate_framebuffer_attachment;

namespace values {
} // namespace values
} // namespace gl::nv::fbo_color_attachments
#endif // GL_NV_fbo_color_attachments
namespace gl::nv::fbo_color_attachments {
constexpr auto name = "GL_NV_fbo_color_attachments";
}
