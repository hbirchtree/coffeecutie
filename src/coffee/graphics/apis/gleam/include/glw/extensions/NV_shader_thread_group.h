#pragma once

#ifdef GL_NV_shader_thread_group
namespace gl::nv::shader_thread_group {
namespace values {
constexpr libc_types::u32 warp_size    = 0x9339;
constexpr libc_types::u32 warps_per_sm = 0x933A;
constexpr libc_types::u32 sm_count     = 0x933B;
} // namespace values
} // namespace gl::nv::shader_thread_group
#endif // GL_NV_shader_thread_group
namespace gl::nv::shader_thread_group {
constexpr auto name = "GL_NV_shader_thread_group";
}
