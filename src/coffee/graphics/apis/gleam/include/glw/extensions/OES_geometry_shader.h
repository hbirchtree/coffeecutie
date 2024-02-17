#pragma once

#ifdef GL_OES_geometry_shader
#include "../enums/FramebufferAttachmentParameterName.h"
#include "../enums/UseProgramStageMask.h"

namespace gl::oes::geometry_shader {
using gl::group::framebuffer_attachment_parameter_name;
using gl::group::use_program_stage_mask;

namespace values {
constexpr u32 lines_adjacency                          = 0x000A;
constexpr u32 line_strip_adjacency                     = 0x000B;
constexpr u32 triangles_adjacency                      = 0x000C;
constexpr u32 triangle_strip_adjacency                 = 0x000D;
constexpr u32 layer_provoking_vertex                   = 0x825E;
constexpr u32 undefined_vertex                         = 0x8260;
constexpr u32 geometry_shader_invocations              = 0x887F;
constexpr u32 geometry_linked_vertices_out             = 0x8916;
constexpr u32 geometry_linked_input_type               = 0x8917;
constexpr u32 geometry_linked_output_type              = 0x8918;
constexpr u32 max_geometry_uniform_blocks              = 0x8A2C;
constexpr u32 max_combined_geometry_uniform_components = 0x8A32;
constexpr u32 max_geometry_texture_image_units         = 0x8C29;
constexpr u32 primitives_generated                     = 0x8C87;
constexpr u32 framebuffer_incomplete_layer_targets     = 0x8DA8;
constexpr u32 geometry_shader                          = 0x8DD9;
constexpr u32 max_geometry_uniform_components          = 0x8DDF;
constexpr u32 max_geometry_output_vertices             = 0x8DE0;
constexpr u32 max_geometry_total_output_components     = 0x8DE1;
constexpr u32 first_vertex_convention                  = 0x8E4D;
constexpr u32 last_vertex_convention                   = 0x8E4E;
constexpr u32 max_geometry_shader_invocations          = 0x8E5A;
constexpr u32 max_geometry_image_uniforms              = 0x90CD;
constexpr u32 max_geometry_shader_storage_blocks       = 0x90D7;
constexpr u32 max_geometry_input_components            = 0x9123;
constexpr u32 max_geometry_output_components           = 0x9124;
constexpr u32 max_geometry_atomic_counter_buffers      = 0x92CF;
constexpr u32 max_geometry_atomic_counters             = 0x92D5;
constexpr u32 referenced_by_geometry_shader            = 0x9309;
constexpr u32 framebuffer_default_layers               = 0x9312;
constexpr u32 max_framebuffer_layers                   = 0x9317;
} // namespace values

/*!
 * \brief Part of GL_OES_geometry_shader
 * \param target GLenum
 * \param attachment GLenum
 * \param texture GLuint
 * \param level GLint
 * \return void
 */
STATICINLINE void framebuffer_texture(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTextureOES)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glFramebufferTextureOES(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        level);
    detail::error_check("FramebufferTextureOES"sv);
}

} // namespace gl::oes::geometry_shader
#endif // GL_OES_geometry_shader
namespace gl::oes::geometry_shader {
constexpr auto name = "GL_OES_geometry_shader";
}
