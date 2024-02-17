#pragma once

#ifdef GL_EXT_sRGB
#include "../enums/FramebufferAttachmentParameterName.h"
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::ext::srgb {
using gl::group::framebuffer_attachment_parameter_name;
using gl::group::internal_format;
using gl::group::sized_internal_format;

namespace values {
} // namespace values
} // namespace gl::ext::srgb
#endif // GL_EXT_sRGB
namespace gl::ext::srgb {
constexpr auto name = "GL_EXT_sRGB";
}
