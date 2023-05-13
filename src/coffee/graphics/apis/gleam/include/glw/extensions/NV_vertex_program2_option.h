#pragma once

#ifdef GL_NV_vertex_program2_option
namespace gl::nv::vertex_program2_option {
namespace values {
constexpr u32 max_program_exec_instructions = 0x88F4;
constexpr u32 max_program_call_depth        = 0x88F5;
} // namespace values
} // namespace gl::nv::vertex_program2_option
#endif // GL_NV_vertex_program2_option
namespace gl::nv::vertex_program2_option {
constexpr auto name = "GL_NV_vertex_program2_option";
}
