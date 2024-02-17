#pragma once

#ifdef GL_NV_geometry_program4
#include "../enums/FramebufferAttachmentParameterName.h"
#include "../enums/PrimitiveType.h"
#include "../enums/ProgramTarget.h"

namespace gl::nv::geometry_program4 {
using gl::group::framebuffer_attachment_parameter_name;
using gl::group::primitive_type;
using gl::group::program_target;

namespace values {
constexpr u32 program_point_size_ext                   = 0x8642;
constexpr u32 max_program_output_vertices              = 0x8C27;
constexpr u32 max_program_total_output_components      = 0x8C28;
constexpr u32 max_geometry_texture_image_units_ext     = 0x8C29;
constexpr u32 framebuffer_incomplete_layer_targets_ext = 0x8DA8;
constexpr u32 framebuffer_incomplete_layer_count_ext   = 0x8DA9;
constexpr u32 geometry_vertices_out_ext                = 0x8DDA;
constexpr u32 geometry_input_type_ext                  = 0x8DDB;
constexpr u32 geometry_output_type_ext                 = 0x8DDC;
} // namespace values

/*!
 * \brief Part of GL_NV_geometry_program4
 * \param target GLenum
 * \param attachment GLenum
 * \param texture GLuint
 * \param level GLint
 * \return void
 */
STATICINLINE void framebuffer_texture_ext(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTextureEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glFramebufferTextureEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        level);
    detail::error_check("FramebufferTextureEXT"sv);
}

/*!
 * \brief Part of GL_NV_geometry_program4
 * \param target GLenum
 * \param attachment GLenum
 * \param texture GLuint
 * \param level GLint
 * \param face GLenum
 * \return void
 */
STATICINLINE void framebuffer_texture_face_ext(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level,
    group::texture_target         face)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTextureFaceEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glFramebufferTextureFaceEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        level,
        static_cast<GLenum>(face));
    detail::error_check("FramebufferTextureFaceEXT"sv);
}

/*!
 * \brief Part of GL_NV_geometry_program4
 * \param target GLenum
 * \param attachment GLenum
 * \param texture GLuint
 * \param level GLint
 * \param layer GLint
 * \return void
 */
STATICINLINE void framebuffer_texture_layer_ext(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level,
    i32                           layer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTextureLayerEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glFramebufferTextureLayerEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        level,
        layer);
    detail::error_check("FramebufferTextureLayerEXT"sv);
}

/*!
 * \brief Part of GL_NV_geometry_program4
 * \param target GLenum
 * \param limit GLint
 * \return void
 */
STATICINLINE void program_vertex_limit(group::program_target target, i32 limit)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(ProgramVertexLimitNV)
    }
    glProgramVertexLimitNV(static_cast<GLenum>(target), limit);
    detail::error_check("ProgramVertexLimitNV"sv);
}

} // namespace gl::nv::geometry_program4
#endif // GL_NV_geometry_program4
namespace gl::nv::geometry_program4 {
constexpr auto name = "GL_NV_geometry_program4";
}
