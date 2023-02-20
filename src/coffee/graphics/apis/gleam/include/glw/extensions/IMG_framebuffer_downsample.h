#pragma once

#ifdef GL_IMG_framebuffer_downsample
#include "../enums/FramebufferAttachmentParameterName.h"
namespace gl::img::framebuffer_downsample {
using gl::group::framebuffer_attachment_parameter_name;
namespace values {
constexpr libc_types::u32 framebuffer_incomplete_multisample_and_downsample =
    0x913C;
constexpr libc_types::u32 num_downsample_scales = 0x913D;
constexpr libc_types::u32 downsample_scales     = 0x913E;
} // namespace values
/*!
 * \brief Part of GL_IMG_framebuffer_downsample
 * \param target GLenum
 * \param attachment GLenum
 * \param textarget GLenum
 * \param texture GLuint
 * \param level GLint
 * \param xscale GLint
 * \param yscale GLint
 * \return void
 */
STATICINLINE void framebuffer_texture_2d_downsample(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    group::texture_target         textarget,
    u32                           texture,
    i32                           level,
    i32                           xscale,
    i32                           yscale)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTexture2DDownsampleIMG)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glFramebufferTexture2DDownsampleIMG(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level,
        xscale,
        yscale);
    detail::error_check("FramebufferTexture2DDownsampleIMG"sv);
}

/*!
 * \brief Part of GL_IMG_framebuffer_downsample
 * \param target GLenum
 * \param attachment GLenum
 * \param texture GLuint
 * \param level GLint
 * \param layer GLint
 * \param xscale GLint
 * \param yscale GLint
 * \return void
 */
STATICINLINE void framebuffer_texture_layer_downsample(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level,
    i32                           layer,
    i32                           xscale,
    i32                           yscale)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTextureLayerDownsampleIMG)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glFramebufferTextureLayerDownsampleIMG(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        level,
        layer,
        xscale,
        yscale);
    detail::error_check("FramebufferTextureLayerDownsampleIMG"sv);
}

} // namespace gl::img::framebuffer_downsample
#endif // GL_IMG_framebuffer_downsample
namespace gl::img::framebuffer_downsample {
constexpr auto name = "GL_IMG_framebuffer_downsample";
}
