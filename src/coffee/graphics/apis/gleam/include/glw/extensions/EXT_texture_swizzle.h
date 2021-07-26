#pragma once

#ifdef GL_EXT_texture_swizzle
namespace gl::ext::texture_swizzle {
namespace values {
constexpr libc_types::u32 texture_swizzle_r    = 0x8E42;
constexpr libc_types::u32 texture_swizzle_g    = 0x8E43;
constexpr libc_types::u32 texture_swizzle_b    = 0x8E44;
constexpr libc_types::u32 texture_swizzle_a    = 0x8E45;
constexpr libc_types::u32 texture_swizzle_rgba = 0x8E46;
} // namespace values
} // namespace gl::ext::texture_swizzle
#endif // GL_EXT_texture_swizzle
namespace gl::ext::texture_swizzle {
constexpr auto name = "GL_EXT_texture_swizzle";
}
