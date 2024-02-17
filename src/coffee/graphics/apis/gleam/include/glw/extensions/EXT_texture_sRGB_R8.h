#pragma once

#ifdef GL_EXT_texture_sRGB_R8
#include "../enums/InternalFormat.h"

namespace gl::ext::texture_srgb_r8 {
using gl::group::internal_format;

namespace values {
} // namespace values
} // namespace gl::ext::texture_srgb_r8
#endif // GL_EXT_texture_sRGB_R8
namespace gl::ext::texture_srgb_r8 {
constexpr auto name = "GL_EXT_texture_sRGB_R8";
}
