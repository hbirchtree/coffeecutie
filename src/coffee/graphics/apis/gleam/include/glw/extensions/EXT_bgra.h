#pragma once

#ifdef GL_EXT_bgra
#include "../enums/PixelFormat.h"
namespace gl::ext::bgra {
using gl::group::pixel_format;
namespace values {
} // namespace values
} // namespace gl::ext::bgra
#endif // GL_EXT_bgra
namespace gl::ext::bgra {
constexpr auto name = "GL_EXT_bgra";
}
