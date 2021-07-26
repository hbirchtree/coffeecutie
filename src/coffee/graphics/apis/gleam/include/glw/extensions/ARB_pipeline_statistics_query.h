#pragma once

#ifdef GL_ARB_pipeline_statistics_query
namespace gl::arb::pipeline_statistics_query {
namespace values {
constexpr libc_types::u32 vertices_submitted                 = 0x82EE;
constexpr libc_types::u32 primitives_submitted               = 0x82EF;
constexpr libc_types::u32 vertex_shader_invocations          = 0x82F0;
constexpr libc_types::u32 tess_control_shader_patches        = 0x82F1;
constexpr libc_types::u32 tess_evaluation_shader_invocations = 0x82F2;
constexpr libc_types::u32 geometry_shader_primitives_emitted = 0x82F3;
constexpr libc_types::u32 fragment_shader_invocations        = 0x82F4;
constexpr libc_types::u32 compute_shader_invocations         = 0x82F5;
constexpr libc_types::u32 clipping_input_primitives          = 0x82F6;
constexpr libc_types::u32 clipping_output_primitives         = 0x82F7;
constexpr libc_types::u32 geometry_shader_invocations        = 0x887F;
} // namespace values
} // namespace gl::arb::pipeline_statistics_query
#endif // GL_ARB_pipeline_statistics_query
namespace gl::arb::pipeline_statistics_query {
constexpr auto name = "GL_ARB_pipeline_statistics_query";
}
