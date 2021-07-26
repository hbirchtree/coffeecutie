#pragma once

#ifdef GL_EXT_abgr
#include "../enums/PixelFormat.h"
namespace gl::ext::abgr {
using gl::group::pixel_format;
namespace values {
} // namespace values
} // namespace gl::ext::abgr
#endif // GL_EXT_abgr
namespace gl::ext::abgr {
constexpr auto name = "GL_EXT_abgr";
}
