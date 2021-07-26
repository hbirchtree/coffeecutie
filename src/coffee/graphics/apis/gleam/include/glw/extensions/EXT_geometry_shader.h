#pragma once

#ifdef GL_EXT_geometry_shader
#include "../enums/FramebufferAttachmentParameterName.h"
#include "../enums/PrimitiveType.h"
#include "../enums/UseProgramStageMask.h"
namespace gl::ext::geometry_shader {
using gl::group::framebuffer_attachment_parameter_name;
using gl::group::primitive_type;
using gl::group::use_program_stage_mask;
namespace values {
constexpr libc_types::u32 layer_provoking_vertex                   = 0x825E;
constexpr libc_types::u32 undefined_vertex                         = 0x8260;
constexpr libc_types::u32 geometry_shader_invocations              = 0x887F;
constexpr libc_types::u32 geometry_linked_vertices_out             = 0x8916;
constexpr libc_types::u32 geometry_linked_input_type               = 0x8917;
constexpr libc_types::u32 geometry_linked_output_type              = 0x8918;
constexpr libc_types::u32 max_geometry_uniform_blocks              = 0x8A2C;
constexpr libc_types::u32 max_combined_geometry_uniform_components = 0x8A32;
constexpr libc_types::u32 max_geometry_texture_image_units         = 0x8C29;
constexpr libc_types::u32 primitives_generated                     = 0x8C87;
constexpr libc_types::u32 framebuffer_incomplete_layer_targets     = 0x8DA8;
constexpr libc_types::u32 geometry_shader                          = 0x8DD9;
constexpr libc_types::u32 max_geometry_uniform_components          = 0x8DDF;
constexpr libc_types::u32 max_geometry_output_vertices             = 0x8DE0;
constexpr libc_types::u32 max_geometry_total_output_components     = 0x8DE1;
constexpr libc_types::u32 first_vertex_convention                  = 0x8E4D;
constexpr libc_types::u32 last_vertex_convention                   = 0x8E4E;
constexpr libc_types::u32 max_geometry_shader_invocations          = 0x8E5A;
constexpr libc_types::u32 max_geometry_image_uniforms              = 0x90CD;
constexpr libc_types::u32 max_geometry_shader_storage_blocks       = 0x90D7;
constexpr libc_types::u32 max_geometry_input_components            = 0x9123;
constexpr libc_types::u32 max_geometry_output_components           = 0x9124;
constexpr libc_types::u32 max_geometry_atomic_counter_buffers      = 0x92CF;
constexpr libc_types::u32 max_geometry_atomic_counters             = 0x92D5;
constexpr libc_types::u32 referenced_by_geometry_shader            = 0x9309;
constexpr libc_types::u32 framebuffer_default_layers               = 0x9312;
constexpr libc_types::u32 max_framebuffer_layers                   = 0x9317;
} // namespace values
STATICINLINE void framebuffer_texture(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTextureEXT)
        glIsTexture(texture);
    }
    glFramebufferTextureEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        level);
    detail::error_check("FramebufferTextureEXT"sv);
}

} // namespace gl::ext::geometry_shader
#endif // GL_EXT_geometry_shader
namespace gl::ext::geometry_shader {
constexpr auto name = "GL_EXT_geometry_shader";
}
