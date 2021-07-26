#pragma once

#ifdef GL_ARB_shadow
namespace gl::arb::shadow {
namespace values {
constexpr libc_types::u32 texture_compare_mode = 0x884C;
constexpr libc_types::u32 texture_compare_func = 0x884D;
constexpr libc_types::u32 compare_r_to_texture = 0x884E;
} // namespace values
} // namespace gl::arb::shadow
#endif // GL_ARB_shadow
namespace gl::arb::shadow {
constexpr auto name = "GL_ARB_shadow";
}
