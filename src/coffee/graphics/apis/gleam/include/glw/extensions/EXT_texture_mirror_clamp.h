#pragma once

#ifdef GL_EXT_texture_mirror_clamp
#include "../enums/TextureWrapMode.h"

namespace gl::ext::texture_mirror_clamp {
using gl::group::texture_wrap_mode;

namespace values {
} // namespace values
} // namespace gl::ext::texture_mirror_clamp
#endif // GL_EXT_texture_mirror_clamp
namespace gl::ext::texture_mirror_clamp {
constexpr auto name = "GL_EXT_texture_mirror_clamp";
}
