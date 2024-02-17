#pragma once

#ifdef GL_ARB_framebuffer_sRGB
#include "../enums/EnableCap.h"

namespace gl::arb::framebuffer_srgb {
using gl::group::enable_cap;

namespace values {
} // namespace values
} // namespace gl::arb::framebuffer_srgb
#endif // GL_ARB_framebuffer_sRGB
namespace gl::arb::framebuffer_srgb {
constexpr auto name = "GL_ARB_framebuffer_sRGB";
}
