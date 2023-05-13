#pragma once

#ifdef GL_ARB_texture_env_dot3
namespace gl::arb::texture_env_dot3 {
namespace values {
constexpr u32 dot3_rgb  = 0x86AE;
constexpr u32 dot3_rgba = 0x86AF;
} // namespace values
} // namespace gl::arb::texture_env_dot3
#endif // GL_ARB_texture_env_dot3
namespace gl::arb::texture_env_dot3 {
constexpr auto name = "GL_ARB_texture_env_dot3";
}
