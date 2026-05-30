#pragma once

#ifdef GL_ARB_texture_mirror_clamp_to_edge
#include "../enums/TextureWrapMode.h"

namespace gl::arb::texture_mirror_clamp_to_edge {
using gl::group::texture_wrap_mode;

namespace values {
} // namespace values
} // namespace gl::arb::texture_mirror_clamp_to_edge
#endif // GL_ARB_texture_mirror_clamp_to_edge
namespace gl::arb::texture_mirror_clamp_to_edge {
constexpr auto name = "GL_ARB_texture_mirror_clamp_to_edge";
}
