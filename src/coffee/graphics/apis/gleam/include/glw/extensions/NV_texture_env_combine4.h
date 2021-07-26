#pragma once

#ifdef GL_NV_texture_env_combine4
namespace gl::nv::texture_env_combine4 {
namespace values {
constexpr libc_types::u32 combine4       = 0x8503;
constexpr libc_types::u32 source3_rgb    = 0x8583;
constexpr libc_types::u32 source3_alpha  = 0x858B;
constexpr libc_types::u32 operand3_rgb   = 0x8593;
constexpr libc_types::u32 operand3_alpha = 0x859B;
} // namespace values
} // namespace gl::nv::texture_env_combine4
#endif // GL_NV_texture_env_combine4
namespace gl::nv::texture_env_combine4 {
constexpr auto name = "GL_NV_texture_env_combine4";
}
