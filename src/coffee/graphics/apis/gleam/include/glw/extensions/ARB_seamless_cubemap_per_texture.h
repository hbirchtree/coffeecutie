#pragma once

#ifdef GL_ARB_seamless_cubemap_per_texture
#include "../enums/EnableCap.h"

namespace gl::arb::seamless_cubemap_per_texture {
using gl::group::enable_cap;

namespace values {
} // namespace values
} // namespace gl::arb::seamless_cubemap_per_texture
#endif // GL_ARB_seamless_cubemap_per_texture
namespace gl::arb::seamless_cubemap_per_texture {
constexpr auto name = "GL_ARB_seamless_cubemap_per_texture";
}
