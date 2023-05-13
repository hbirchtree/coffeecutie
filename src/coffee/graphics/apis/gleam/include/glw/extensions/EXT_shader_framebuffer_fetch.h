#pragma once

#ifdef GL_EXT_shader_framebuffer_fetch
namespace gl::ext::shader_framebuffer_fetch {
namespace values {
constexpr u32 fragment_shader_discards_samples = 0x8A52;
} // namespace values
} // namespace gl::ext::shader_framebuffer_fetch
#endif // GL_EXT_shader_framebuffer_fetch
namespace gl::ext::shader_framebuffer_fetch {
constexpr auto name = "GL_EXT_shader_framebuffer_fetch";
}
