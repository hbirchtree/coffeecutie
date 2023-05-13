#pragma once

#ifdef GL_EXT_framebuffer_multisample_blit_scaled
namespace gl::ext::framebuffer_multisample_blit_scaled {
namespace values {
constexpr u32 scaled_resolve_fastest = 0x90BA;
constexpr u32 scaled_resolve_nicest  = 0x90BB;
} // namespace values
} // namespace gl::ext::framebuffer_multisample_blit_scaled
#endif // GL_EXT_framebuffer_multisample_blit_scaled
namespace gl::ext::framebuffer_multisample_blit_scaled {
constexpr auto name = "GL_EXT_framebuffer_multisample_blit_scaled";
}
