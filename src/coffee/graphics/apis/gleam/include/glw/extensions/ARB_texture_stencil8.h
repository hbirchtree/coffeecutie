#pragma once

#ifdef GL_ARB_texture_stencil8
#include "../enums/InternalFormat.h"
#include "../enums/PixelFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::arb::texture_stencil8 {
using gl::group::internal_format;
using gl::group::pixel_format;
using gl::group::sized_internal_format;
namespace values {
} // namespace values
} // namespace gl::arb::texture_stencil8
#endif // GL_ARB_texture_stencil8
namespace gl::arb::texture_stencil8 {
constexpr auto name = "GL_ARB_texture_stencil8";
}
