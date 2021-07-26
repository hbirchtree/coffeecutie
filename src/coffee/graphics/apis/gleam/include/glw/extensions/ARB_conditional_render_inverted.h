#pragma once

#ifdef GL_ARB_conditional_render_inverted
#include "../enums/ConditionalRenderMode.h"
namespace gl::arb::conditional_render_inverted {
using gl::group::conditional_render_mode;
namespace values {
} // namespace values
} // namespace gl::arb::conditional_render_inverted
#endif // GL_ARB_conditional_render_inverted
namespace gl::arb::conditional_render_inverted {
constexpr auto name = "GL_ARB_conditional_render_inverted";
}
