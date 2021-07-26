#pragma once

#ifdef GL_ARM_shader_framebuffer_fetch
namespace gl::arm::shader_framebuffer_fetch {
namespace values {
constexpr libc_types::u32 fetch_per_sample                      = 0x8F65;
constexpr libc_types::u32 fragment_shader_framebuffer_fetch_mrt = 0x8F66;
} // namespace values
} // namespace gl::arm::shader_framebuffer_fetch
#endif // GL_ARM_shader_framebuffer_fetch
namespace gl::arm::shader_framebuffer_fetch {
constexpr auto name = "GL_ARM_shader_framebuffer_fetch";
}
