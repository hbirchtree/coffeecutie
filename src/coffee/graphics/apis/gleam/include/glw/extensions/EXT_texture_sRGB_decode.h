#pragma once

#ifdef GL_EXT_texture_sRGB_decode
namespace gl::ext::texture_srgb_decode {
namespace values {
constexpr u32 texture_srgb_decode = 0x8A48;
constexpr u32 decode              = 0x8A49;
constexpr u32 skip_decode         = 0x8A4A;
} // namespace values
} // namespace gl::ext::texture_srgb_decode
#endif // GL_EXT_texture_sRGB_decode
namespace gl::ext::texture_srgb_decode {
constexpr auto name = "GL_EXT_texture_sRGB_decode";
}
