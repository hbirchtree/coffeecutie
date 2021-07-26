#pragma once

#ifdef GL_IMG_texture_compression_pvrtc2
namespace gl::img::texture_compression_pvrtc2 {
namespace values {
constexpr libc_types::u32 compressed_rgba_pvrtc_2bppv2 = 0x9137;
constexpr libc_types::u32 compressed_rgba_pvrtc_4bppv2 = 0x9138;
} // namespace values
} // namespace gl::img::texture_compression_pvrtc2
#endif // GL_IMG_texture_compression_pvrtc2
namespace gl::img::texture_compression_pvrtc2 {
constexpr auto name = "GL_IMG_texture_compression_pvrtc2";
}
