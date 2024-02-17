#pragma once

#ifdef GL_ARB_texture_swizzle
#include "../enums/TextureParameterName.h"

namespace gl::arb::texture_swizzle {
using gl::group::texture_parameter_name;

namespace values {
} // namespace values
} // namespace gl::arb::texture_swizzle
#endif // GL_ARB_texture_swizzle
namespace gl::arb::texture_swizzle {
constexpr auto name = "GL_ARB_texture_swizzle";
}
