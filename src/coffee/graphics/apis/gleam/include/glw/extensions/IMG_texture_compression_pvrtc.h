#pragma once

#ifdef GL_IMG_texture_compression_pvrtc
namespace gl::img::texture_compression_pvrtc {
namespace values {
constexpr libc_types::u32 compressed_rgb_pvrtc_4bppv1  = 0x8C00;
constexpr libc_types::u32 compressed_rgb_pvrtc_2bppv1  = 0x8C01;
constexpr libc_types::u32 compressed_rgba_pvrtc_4bppv1 = 0x8C02;
constexpr libc_types::u32 compressed_rgba_pvrtc_2bppv1 = 0x8C03;
} // namespace values
} // namespace gl::img::texture_compression_pvrtc
#endif // GL_IMG_texture_compression_pvrtc
namespace gl::img::texture_compression_pvrtc {
constexpr auto name = "GL_IMG_texture_compression_pvrtc";
}
