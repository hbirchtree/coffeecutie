#pragma once

#ifdef GL_ARB_seamless_cube_map
#include "../enums/EnableCap.h"

namespace gl::arb::seamless_cube_map {
using gl::group::enable_cap;

namespace values {
} // namespace values
} // namespace gl::arb::seamless_cube_map
#endif // GL_ARB_seamless_cube_map
namespace gl::arb::seamless_cube_map {
constexpr auto name = "GL_ARB_seamless_cube_map";
}
