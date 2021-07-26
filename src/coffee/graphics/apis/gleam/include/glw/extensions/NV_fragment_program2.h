#pragma once

#ifdef GL_NV_fragment_program2
namespace gl::nv::fragment_program2 {
namespace values {
constexpr libc_types::u32 max_program_exec_instructions = 0x88F4;
constexpr libc_types::u32 max_program_call_depth        = 0x88F5;
constexpr libc_types::u32 max_program_if_depth          = 0x88F6;
constexpr libc_types::u32 max_program_loop_depth        = 0x88F7;
constexpr libc_types::u32 max_program_loop_count        = 0x88F8;
} // namespace values
} // namespace gl::nv::fragment_program2
#endif // GL_NV_fragment_program2
namespace gl::nv::fragment_program2 {
constexpr auto name = "GL_NV_fragment_program2";
}
