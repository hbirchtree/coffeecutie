#pragma once

#ifdef GL_AMD_seamless_cubemap_per_texture
#include "../enums/EnableCap.h"

namespace gl::amd::seamless_cubemap_per_texture {
using gl::group::enable_cap;

namespace values {
} // namespace values
} // namespace gl::amd::seamless_cubemap_per_texture
#endif // GL_AMD_seamless_cubemap_per_texture
namespace gl::amd::seamless_cubemap_per_texture {
constexpr auto name = "GL_AMD_seamless_cubemap_per_texture";
}
