#pragma once

#ifdef GL_ARB_half_float_pixel
#include "../enums/PixelType.h"

namespace gl::arb::half_float_pixel {
using gl::group::pixel_type;

namespace values {
} // namespace values
} // namespace gl::arb::half_float_pixel
#endif // GL_ARB_half_float_pixel
namespace gl::arb::half_float_pixel {
constexpr auto name = "GL_ARB_half_float_pixel";
}
