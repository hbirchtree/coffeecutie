#pragma once

#ifdef GL_ARB_texture_rg
#include "../enums/InternalFormat.h"
#include "../enums/PixelFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::arb::texture_rg {
using gl::group::internal_format;
using gl::group::pixel_format;
using gl::group::sized_internal_format;

namespace values {
} // namespace values
} // namespace gl::arb::texture_rg
#endif // GL_ARB_texture_rg
namespace gl::arb::texture_rg {
constexpr auto name = "GL_ARB_texture_rg";
}
