#pragma once

#ifdef GL_EXT_framebuffer_sRGB
namespace gl::ext::framebuffer_srgb {
namespace values {
constexpr u32 framebuffer_srgb         = 0x8DB9;
constexpr u32 framebuffer_srgb_capable = 0x8DBA;
} // namespace values
} // namespace gl::ext::framebuffer_srgb
#endif // GL_EXT_framebuffer_sRGB
namespace gl::ext::framebuffer_srgb {
constexpr auto name = "GL_EXT_framebuffer_sRGB";
}
