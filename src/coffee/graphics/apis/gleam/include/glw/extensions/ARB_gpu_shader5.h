#pragma once

#ifdef GL_ARB_gpu_shader5
namespace gl::arb::gpu_shader5 {
namespace values {
constexpr libc_types::u32 geometry_shader_invocations        = 0x887F;
constexpr libc_types::u32 max_geometry_shader_invocations    = 0x8E5A;
constexpr libc_types::u32 min_fragment_interpolation_offset  = 0x8E5B;
constexpr libc_types::u32 max_fragment_interpolation_offset  = 0x8E5C;
constexpr libc_types::u32 fragment_interpolation_offset_bits = 0x8E5D;
constexpr libc_types::u32 max_vertex_streams                 = 0x8E71;
} // namespace values
} // namespace gl::arb::gpu_shader5
#endif // GL_ARB_gpu_shader5
namespace gl::arb::gpu_shader5 {
constexpr auto name = "GL_ARB_gpu_shader5";
}
