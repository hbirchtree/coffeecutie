#pragma once

#ifdef GL_EXT_texture_format_BGRA8888
#include "../enums/PixelFormat.h"

namespace gl::ext::texture_format_bgra8888 {
using gl::group::pixel_format;

namespace values {
} // namespace values
} // namespace gl::ext::texture_format_bgra8888
#endif // GL_EXT_texture_format_BGRA8888
namespace gl::ext::texture_format_bgra8888 {
constexpr auto name = "GL_EXT_texture_format_BGRA8888";
}
