#pragma once

#ifdef GL_EXT_texture_mirror_clamp_to_edge
#include "../enums/TextureWrapMode.h"

namespace gl::ext::texture_mirror_clamp_to_edge {
using gl::group::texture_wrap_mode;

namespace values {
} // namespace values
} // namespace gl::ext::texture_mirror_clamp_to_edge
#endif // GL_EXT_texture_mirror_clamp_to_edge
namespace gl::ext::texture_mirror_clamp_to_edge {
constexpr auto name = "GL_EXT_texture_mirror_clamp_to_edge";
}
