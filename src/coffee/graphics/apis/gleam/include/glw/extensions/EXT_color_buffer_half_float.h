#pragma once

#ifdef GL_EXT_color_buffer_half_float
#include "../enums/FramebufferAttachmentParameterName.h"
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::ext::color_buffer_half_float {
using gl::group::framebuffer_attachment_parameter_name;
using gl::group::internal_format;
using gl::group::sized_internal_format;

namespace values {
constexpr u32 unsigned_normalized = 0x8C17;
} // namespace values
} // namespace gl::ext::color_buffer_half_float
#endif // GL_EXT_color_buffer_half_float
namespace gl::ext::color_buffer_half_float {
constexpr auto name = "GL_EXT_color_buffer_half_float";
}
