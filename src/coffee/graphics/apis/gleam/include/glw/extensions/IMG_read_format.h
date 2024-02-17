#pragma once

#ifdef GL_IMG_read_format
#include "../enums/PixelFormat.h"
#include "../enums/PixelType.h"

namespace gl::img::read_format {
using gl::group::pixel_format;
using gl::group::pixel_type;

namespace values {
} // namespace values
} // namespace gl::img::read_format
#endif // GL_IMG_read_format
namespace gl::img::read_format {
constexpr auto name = "GL_IMG_read_format";
}
