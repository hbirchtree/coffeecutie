#pragma once

#ifdef GL_EXT_multisampled_render_to_texture
#include "../enums/FramebufferAttachmentParameterName.h"
#include "../enums/RenderbufferParameterName.h"
namespace gl::ext::multisampled_render_to_texture {
using gl::group::framebuffer_attachment_parameter_name;
using gl::group::renderbuffer_parameter_name;
namespace values {
constexpr libc_types::u32 framebuffer_incomplete_multisample = 0x8D56;
constexpr libc_types::u32 max_samples                        = 0x8D57;
} // namespace values
/*!
 * \brief Part of GL_EXT_multisampled_render_to_texture
 * \param target GLenum
 * \param attachment GLenum
 * \param textarget GLenum
 * \param texture GLuint
 * \param level GLint
 * \param samples GLsizei
 * \return void
 */
STATICINLINE void framebuffer_texture_2d_multisample(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    group::texture_target         textarget,
    u32                           texture,
    i32                           level,
    i32                           samples)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTexture2DMultisampleEXT)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glFramebufferTexture2DMultisampleEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level,
        samples);
    detail::error_check("FramebufferTexture2DMultisampleEXT"sv);
}

template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>)
    /*!
     * \brief Part of GL_EXT_multisampled_render_to_texture
     * \param target GLenum
     * \param samples GLsizei
     * \param internalformat GLenum
     * \param width GLsizei
     * \param height GLsizei
     * \return void
     */
    STATICINLINE void renderbuffer_storage_multisample(
        group::renderbuffer_target target,
        i32                        samples,
        group::internal_format     internalformat,
        size_2_i32 const&          width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RenderbufferStorageMultisampleEXT)
    }
    glRenderbufferStorageMultisampleEXT(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("RenderbufferStorageMultisampleEXT"sv);
}

} // namespace gl::ext::multisampled_render_to_texture
#endif // GL_EXT_multisampled_render_to_texture
namespace gl::ext::multisampled_render_to_texture {
constexpr auto name = "GL_EXT_multisampled_render_to_texture";
}
