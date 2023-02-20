#pragma once

#ifdef GL_EXT_geometry_shader4
#include "../enums/FramebufferAttachmentParameterName.h"
#include "../enums/PrimitiveType.h"
namespace gl::ext::geometry_shader4 {
using gl::group::framebuffer_attachment_parameter_name;
using gl::group::primitive_type;
namespace values {
constexpr libc_types::u32 program_point_size                   = 0x8642;
constexpr libc_types::u32 max_varying_components               = 0x8B4B;
constexpr libc_types::u32 max_geometry_texture_image_units     = 0x8C29;
constexpr libc_types::u32 framebuffer_incomplete_layer_targets = 0x8DA8;
constexpr libc_types::u32 framebuffer_incomplete_layer_count   = 0x8DA9;
constexpr libc_types::u32 geometry_shader                      = 0x8DD9;
constexpr libc_types::u32 geometry_vertices_out                = 0x8DDA;
constexpr libc_types::u32 geometry_input_type                  = 0x8DDB;
constexpr libc_types::u32 geometry_output_type                 = 0x8DDC;
constexpr libc_types::u32 max_geometry_varying_components      = 0x8DDD;
constexpr libc_types::u32 max_vertex_varying_components        = 0x8DDE;
constexpr libc_types::u32 max_geometry_uniform_components      = 0x8DDF;
constexpr libc_types::u32 max_geometry_output_vertices         = 0x8DE0;
constexpr libc_types::u32 max_geometry_total_output_components = 0x8DE1;
} // namespace values
/*!
 * \brief Part of GL_EXT_geometry_shader4
 * \param program GLuint
 * \param pname GLenum
 * \param value GLint
 * \return void
 */
STATICINLINE void program_parameter(
    u32 program, group::program_parameter_prop pname, i32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramParameteriEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsProgram)
            glIsProgram(program);
#endif
    }
    glProgramParameteriEXT(program, static_cast<GLenum>(pname), value);
    detail::error_check("ProgramParameteriEXT"sv);
}

} // namespace gl::ext::geometry_shader4
#endif // GL_EXT_geometry_shader4
namespace gl::ext::geometry_shader4 {
constexpr auto name = "GL_EXT_geometry_shader4";
}
