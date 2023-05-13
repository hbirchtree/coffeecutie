#pragma once

#ifdef GL_ARB_pipeline_statistics_query
namespace gl::arb::pipeline_statistics_query {
namespace values {
constexpr u32 vertices_submitted                 = 0x82EE;
constexpr u32 primitives_submitted               = 0x82EF;
constexpr u32 vertex_shader_invocations          = 0x82F0;
constexpr u32 tess_control_shader_patches        = 0x82F1;
constexpr u32 tess_evaluation_shader_invocations = 0x82F2;
constexpr u32 geometry_shader_primitives_emitted = 0x82F3;
constexpr u32 fragment_shader_invocations        = 0x82F4;
constexpr u32 compute_shader_invocations         = 0x82F5;
constexpr u32 clipping_input_primitives          = 0x82F6;
constexpr u32 clipping_output_primitives         = 0x82F7;
constexpr u32 geometry_shader_invocations        = 0x887F;
} // namespace values
} // namespace gl::arb::pipeline_statistics_query
#endif // GL_ARB_pipeline_statistics_query
namespace gl::arb::pipeline_statistics_query {
constexpr auto name = "GL_ARB_pipeline_statistics_query";
}
