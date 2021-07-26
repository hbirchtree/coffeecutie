#pragma once

#ifdef GL_ARB_depth_clamp
#include "../enums/EnableCap.h"
namespace gl::arb::depth_clamp {
using gl::group::enable_cap;
namespace values {
} // namespace values
} // namespace gl::arb::depth_clamp
#endif // GL_ARB_depth_clamp
namespace gl::arb::depth_clamp {
constexpr auto name = "GL_ARB_depth_clamp";
}
