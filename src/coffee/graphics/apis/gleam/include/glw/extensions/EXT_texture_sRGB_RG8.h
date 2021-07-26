#pragma once

#ifdef GL_EXT_texture_sRGB_RG8
#include "../enums/InternalFormat.h"
namespace gl::ext::texture_srgb_rg8 {
using gl::group::internal_format;
namespace values {
} // namespace values
} // namespace gl::ext::texture_srgb_rg8
#endif // GL_EXT_texture_sRGB_RG8
namespace gl::ext::texture_srgb_rg8 {
constexpr auto name = "GL_EXT_texture_sRGB_RG8";
}
