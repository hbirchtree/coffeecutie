#pragma once

#ifdef GL_OES_compressed_paletted_texture
namespace gl::oes::compressed_paletted_texture {
namespace values {
constexpr libc_types::u32 palette4_rgb8     = 0x8B90;
constexpr libc_types::u32 palette4_rgba8    = 0x8B91;
constexpr libc_types::u32 palette4_r5_g6_b5 = 0x8B92;
constexpr libc_types::u32 palette4_rgba4    = 0x8B93;
constexpr libc_types::u32 palette4_rgb5_a1  = 0x8B94;
constexpr libc_types::u32 palette8_rgb8     = 0x8B95;
constexpr libc_types::u32 palette8_rgba8    = 0x8B96;
constexpr libc_types::u32 palette8_r5_g6_b5 = 0x8B97;
constexpr libc_types::u32 palette8_rgba4    = 0x8B98;
constexpr libc_types::u32 palette8_rgb5_a1  = 0x8B99;
} // namespace values
} // namespace gl::oes::compressed_paletted_texture
#endif // GL_OES_compressed_paletted_texture
namespace gl::oes::compressed_paletted_texture {
constexpr auto name = "GL_OES_compressed_paletted_texture";
}
