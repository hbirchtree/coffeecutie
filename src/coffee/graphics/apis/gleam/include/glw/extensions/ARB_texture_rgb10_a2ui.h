#pragma once

#ifdef GL_ARB_texture_rgb10_a2ui
#include "../enums/InternalFormat.h"
#include "../enums/SizedInternalFormat.h"

namespace gl::arb::texture_rgb10_a2ui {
using gl::group::internal_format;
using gl::group::sized_internal_format;

namespace values {
} // namespace values
} // namespace gl::arb::texture_rgb10_a2ui
#endif // GL_ARB_texture_rgb10_a2ui
namespace gl::arb::texture_rgb10_a2ui {
constexpr auto name = "GL_ARB_texture_rgb10_a2ui";
}
