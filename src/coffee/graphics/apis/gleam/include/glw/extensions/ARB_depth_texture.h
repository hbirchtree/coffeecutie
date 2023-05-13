#pragma once

#ifdef GL_ARB_depth_texture
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::arb::depth_texture {
using gl::group::internal_format;
using gl::group::sized_internal_format;
namespace values {
constexpr u32 texture_depth_size = 0x884A;
constexpr u32 depth_texture_mode = 0x884B;
} // namespace values
} // namespace gl::arb::depth_texture
#endif // GL_ARB_depth_texture
namespace gl::arb::depth_texture {
constexpr auto name = "GL_ARB_depth_texture";
}
