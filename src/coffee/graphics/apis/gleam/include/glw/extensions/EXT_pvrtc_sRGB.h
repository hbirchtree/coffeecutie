#pragma once

#ifdef GL_EXT_pvrtc_sRGB
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::ext::pvrtc_srgb {
using gl::group::internal_format;
using gl::group::sized_internal_format;

namespace values {
} // namespace values
} // namespace gl::ext::pvrtc_srgb
#endif // GL_EXT_pvrtc_sRGB
namespace gl::ext::pvrtc_srgb {
constexpr auto name = "GL_EXT_pvrtc_sRGB";
}
