#pragma once

#ifdef GL_ARB_shadow_ambient
namespace gl::arb::shadow_ambient {
namespace values {
constexpr u32 texture_compare_fail_value = 0x80BF;
} // namespace values
} // namespace gl::arb::shadow_ambient
#endif // GL_ARB_shadow_ambient
namespace gl::arb::shadow_ambient {
constexpr auto name = "GL_ARB_shadow_ambient";
}
