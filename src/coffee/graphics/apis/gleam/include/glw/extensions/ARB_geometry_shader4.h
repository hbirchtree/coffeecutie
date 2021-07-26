#pragma once

#ifdef GL_ARB_geometry_shader4
#include "../enums/FramebufferAttachmentParameterName.h"
#include "../enums/GetPName.h"
#include "../enums/PrimitiveType.h"
namespace gl::arb::geometry_shader4 {
using gl::group::framebuffer_attachment_parameter_name;
using gl::group::get_prop;
using gl::group::primitive_type;
namespace values {
constexpr libc_types::u32 program_point_size                   = 0x8642;
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
STATICINLINE void framebuffer_texture(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTextureARB)
        glIsTexture(texture);
    }
    glFramebufferTextureARB(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        level);
    detail::error_check("FramebufferTextureARB"sv);
}

STATICINLINE void framebuffer_texture_face(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level,
    group::texture_target         face)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTextureFaceARB)
        glIsTexture(texture);
    }
    glFramebufferTextureFaceARB(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        level,
        static_cast<GLenum>(face));
    detail::error_check("FramebufferTextureFaceARB"sv);
}

STATICINLINE void framebuffer_texture_layer(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level,
    i32                           layer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTextureLayerARB)
        glIsTexture(texture);
    }
    glFramebufferTextureLayerARB(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        level,
        layer);
    detail::error_check("FramebufferTextureLayerARB"sv);
}

STATICINLINE void program_parameter(
    u32 program, group::program_parameter_prop pname, i32 value)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramParameteriARB)
        glIsProgram(program);
    }
    glProgramParameteriARB(program, static_cast<GLenum>(pname), value);
    detail::error_check("ProgramParameteriARB"sv);
}

} // namespace gl::arb::geometry_shader4
#endif // GL_ARB_geometry_shader4
namespace gl::arb::geometry_shader4 {
constexpr auto name = "GL_ARB_geometry_shader4";
}
