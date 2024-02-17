#pragma once

#ifdef GL_EXT_read_format_bgra
#include "../enums/PixelFormat.h"
#include "../enums/PixelType.h"

namespace gl::ext::read_format_bgra {
using gl::group::pixel_format;
using gl::group::pixel_type;

namespace values {
} // namespace values
} // namespace gl::ext::read_format_bgra
#endif // GL_EXT_read_format_bgra
namespace gl::ext::read_format_bgra {
constexpr auto name = "GL_EXT_read_format_bgra";
}
