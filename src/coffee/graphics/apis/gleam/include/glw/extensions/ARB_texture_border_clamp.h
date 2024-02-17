#pragma once

#ifdef GL_ARB_texture_border_clamp
#include "../enums/TextureWrapMode.h"

namespace gl::arb::texture_border_clamp {
using gl::group::texture_wrap_mode;

namespace values {
} // namespace values
} // namespace gl::arb::texture_border_clamp
#endif // GL_ARB_texture_border_clamp
namespace gl::arb::texture_border_clamp {
constexpr auto name = "GL_ARB_texture_border_clamp";
}
