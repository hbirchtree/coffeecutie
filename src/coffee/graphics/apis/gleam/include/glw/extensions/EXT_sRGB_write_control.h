#pragma once

#ifdef GL_EXT_sRGB_write_control
namespace gl::ext::srgb_write_control {
namespace values {
constexpr libc_types::u32 framebuffer_srgb = 0x8DB9;
} // namespace values
} // namespace gl::ext::srgb_write_control
#endif // GL_EXT_sRGB_write_control
namespace gl::ext::srgb_write_control {
constexpr auto name = "GL_EXT_sRGB_write_control";
}
