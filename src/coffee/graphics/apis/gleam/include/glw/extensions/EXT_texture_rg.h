#pragma once

#ifdef GL_EXT_texture_rg
#include "../enums/InternalFormat.h"
#include "../enums/PixelFormat.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::ext::texture_rg {
using gl::group::internal_format;
using gl::group::pixel_format;
using gl::group::sized_internal_format;
namespace values {
constexpr u32 rg = 0x8227;
} // namespace values
} // namespace gl::ext::texture_rg
#endif // GL_EXT_texture_rg
namespace gl::ext::texture_rg {
constexpr auto name = "GL_EXT_texture_rg";
}
