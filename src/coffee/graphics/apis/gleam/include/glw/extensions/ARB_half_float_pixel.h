#pragma once

#ifdef GL_ARB_half_float_pixel
namespace gl::arb::half_float_pixel {
namespace values {
constexpr libc_types::u32 half_float = 0x140B;
} // namespace values
} // namespace gl::arb::half_float_pixel
#endif // GL_ARB_half_float_pixel
namespace gl::arb::half_float_pixel {
constexpr auto name = "GL_ARB_half_float_pixel";
}
