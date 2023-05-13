#pragma once

#ifdef GL_EXT_texture_compression_latc
namespace gl::ext::texture_compression_latc {
namespace values {
constexpr u32 compressed_luminance_latc1              = 0x8C70;
constexpr u32 compressed_signed_luminance_latc1       = 0x8C71;
constexpr u32 compressed_luminance_alpha_latc2        = 0x8C72;
constexpr u32 compressed_signed_luminance_alpha_latc2 = 0x8C73;
} // namespace values
} // namespace gl::ext::texture_compression_latc
#endif // GL_EXT_texture_compression_latc
namespace gl::ext::texture_compression_latc {
constexpr auto name = "GL_EXT_texture_compression_latc";
}
