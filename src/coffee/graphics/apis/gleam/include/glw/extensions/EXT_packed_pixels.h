#pragma once

#ifdef GL_EXT_packed_pixels
#include "../enums/PixelType.h"

namespace gl::ext::packed_pixels {
using gl::group::pixel_type;

namespace values {
} // namespace values
} // namespace gl::ext::packed_pixels
#endif // GL_EXT_packed_pixels
namespace gl::ext::packed_pixels {
constexpr auto name = "GL_EXT_packed_pixels";
}
