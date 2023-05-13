#pragma once

#ifdef GL_EXT_texture_format_sRGB_override
namespace gl::ext::texture_format_srgb_override {
namespace values {
constexpr u32 texture_format_srgb_override = 0x8FBF;
} // namespace values
} // namespace gl::ext::texture_format_srgb_override
#endif // GL_EXT_texture_format_sRGB_override
namespace gl::ext::texture_format_srgb_override {
constexpr auto name = "GL_EXT_texture_format_sRGB_override";
}
