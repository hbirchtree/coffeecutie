#pragma once

#ifdef GL_EXT_texture_compression_astc_decode_mode
namespace gl::ext::texture_compression_astc_decode_mode {
namespace values {
constexpr libc_types::u32 texture_astc_decode_precision = 0x8F69;
} // namespace values
} // namespace gl::ext::texture_compression_astc_decode_mode
#endif // GL_EXT_texture_compression_astc_decode_mode
namespace gl::ext::texture_compression_astc_decode_mode {
constexpr auto name = "GL_EXT_texture_compression_astc_decode_mode";
}
