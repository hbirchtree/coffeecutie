#pragma once

#ifdef GL_NV_float_buffer
namespace gl::nv::float_buffer {
namespace values {
constexpr libc_types::u32 float_r                  = 0x8880;
constexpr libc_types::u32 float_rg                 = 0x8881;
constexpr libc_types::u32 float_rgb                = 0x8882;
constexpr libc_types::u32 float_rgba               = 0x8883;
constexpr libc_types::u32 float_r16                = 0x8884;
constexpr libc_types::u32 float_r32                = 0x8885;
constexpr libc_types::u32 float_rg16               = 0x8886;
constexpr libc_types::u32 float_rg32               = 0x8887;
constexpr libc_types::u32 float_rgb16              = 0x8888;
constexpr libc_types::u32 float_rgb32              = 0x8889;
constexpr libc_types::u32 float_rgba16             = 0x888A;
constexpr libc_types::u32 float_rgba32             = 0x888B;
constexpr libc_types::u32 texture_float_components = 0x888C;
constexpr libc_types::u32 float_clear_color_value  = 0x888D;
constexpr libc_types::u32 float_rgba_mode          = 0x888E;
} // namespace values
} // namespace gl::nv::float_buffer
#endif // GL_NV_float_buffer
namespace gl::nv::float_buffer {
constexpr auto name = "GL_NV_float_buffer";
}
