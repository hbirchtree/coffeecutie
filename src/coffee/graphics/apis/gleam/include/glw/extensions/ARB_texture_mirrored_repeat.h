#pragma once

#ifdef GL_ARB_texture_mirrored_repeat
#include "../enums/TextureWrapMode.h"

namespace gl::arb::texture_mirrored_repeat {
using gl::group::texture_wrap_mode;

namespace values {
} // namespace values
} // namespace gl::arb::texture_mirrored_repeat
#endif // GL_ARB_texture_mirrored_repeat
namespace gl::arb::texture_mirrored_repeat {
constexpr auto name = "GL_ARB_texture_mirrored_repeat";
}
