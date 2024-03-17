#pragma once

#ifdef GL_ARB_framebuffer_object
#include "../enums/ColorBuffer.h"
#include "../enums/CopyImageSubDataTarget.h"
#include "../enums/DrawBufferMode.h"
#include "../enums/ErrorCode.h"
#include "../enums/FramebufferAttachment.h"
#include "../enums/FramebufferAttachmentParameterName.h"
#include "../enums/FramebufferStatus.h"
#include "../enums/FramebufferTarget.h"
#include "../enums/GetPName.h"
#include "../enums/InternalFormat.h"
#include "../enums/InvalidateFramebufferAttachment.h"
#include "../enums/ObjectIdentifier.h"
#include "../enums/PixelFormat.h"
#include "../enums/PixelType.h"
#include "../enums/ReadBufferMode.h"
#include "../enums/RenderbufferParameterName.h"
#include "../enums/RenderbufferTarget.h"
#include "../enums/SizedInternalFormat.h"
#include "../enums/TextureTarget.h"

namespace gl::arb::framebuffer_object {
using gl::group::color_buffer;
using gl::group::copy_image_sub_data_target;
using gl::group::draw_buffer_mode;
using gl::group::error_code;
using gl::group::framebuffer_attachment;
using gl::group::framebuffer_attachment_parameter_name;
using gl::group::framebuffer_status;
using gl::group::framebuffer_target;
using gl::group::get_prop;
using gl::group::internal_format;
using gl::group::invalidate_framebuffer_attachment;
using gl::group::object_identifier;
using gl::group::pixel_format;
using gl::group::pixel_type;
using gl::group::read_buffer_mode;
using gl::group::renderbuffer_parameter_name;
using gl::group::renderbuffer_target;
using gl::group::sized_internal_format;
using gl::group::texture_target;

namespace values {
constexpr u32 framebuffer_default = 0x8218;
#if defined(GL_VERSION_1_0)
constexpr u32 index = 0x8222;
#endif
constexpr u32 texture_stencil_size = 0x88F1;
constexpr u32 unsigned_normalized  = 0x8C17;
constexpr u32 max_samples          = 0x8D57;
} // namespace values

/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param target GLenum
 * \param framebuffer GLuint
 * \return void
 */
STATICINLINE void bind_framebuffer(
    group::framebuffer_target target,
    u32                       framebuffer,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindFramebuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    glBindFramebuffer(static_cast<GLenum>(target), framebuffer);
    detail::error_check("BindFramebuffer"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param target GLenum
 * \param renderbuffer GLuint
 * \return void
 */
STATICINLINE void bind_renderbuffer(
    group::renderbuffer_target target,
    u32                        renderbuffer,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindRenderbuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    glBindRenderbuffer(static_cast<GLenum>(target), renderbuffer);
    detail::error_check("BindRenderbuffer"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param srcX0 GLint
 * \param srcY0 GLint
 * \param srcX1 GLint
 * \param srcY1 GLint
 * \param dstX0 GLint
 * \param dstY0 GLint
 * \param dstX1 GLint
 * \param dstY1 GLint
 * \param mask GLbitfield
 * \param filter GLenum
 * \return void
 */
STATICINLINE void blit_framebuffer(
    i32                            srcX0,
    i32                            srcY0,
    i32                            srcX1,
    i32                            srcY1,
    i32                            dstX0,
    i32                            dstY0,
    i32                            dstX1,
    i32                            dstY1,
    group::clear_buffer_mask       mask,
    group::blit_framebuffer_filter filter,
    error_check                    check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BlitFramebuffer)
    }
    glBlitFramebuffer(
        srcX0,
        srcY0,
        srcX1,
        srcY1,
        dstX0,
        dstY0,
        dstX1,
        dstY1,
        static_cast<GLenum>(mask),
        static_cast<GLenum>(filter));
    detail::error_check("BlitFramebuffer"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param target GLenum
 * \return FramebufferStatus
 */
STATICINLINE group::framebuffer_status check_framebuffer_status(
    group::framebuffer_target target,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CheckFramebufferStatus)
    }
    auto out = glCheckFramebufferStatus(static_cast<GLenum>(target));
    detail::error_check("CheckFramebufferStatus"sv, check_errors);
    return static_cast<group::framebuffer_status>(out);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param n GLsizei
 * \param framebuffers const GLuint *
 * \return void
 */
STATICINLINE void delete_framebuffers(
    span_const_u32 const& framebuffers,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteFramebuffers)
    }
    glDeleteFramebuffers(
        framebuffers.size(),
        framebuffers.size()
            ? reinterpret_cast<const GLuint*>(framebuffers.data())
            : nullptr);
    detail::error_check("DeleteFramebuffers"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param n GLsizei
 * \param renderbuffers const GLuint *
 * \return void
 */
STATICINLINE void delete_renderbuffers(
    span_const_u32 const& renderbuffers,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteRenderbuffers)
    }
    glDeleteRenderbuffers(
        renderbuffers.size(),
        renderbuffers.size()
            ? reinterpret_cast<const GLuint*>(renderbuffers.data())
            : nullptr);
    detail::error_check("DeleteRenderbuffers"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param target GLenum
 * \param attachment GLenum
 * \param renderbuffertarget GLenum
 * \param renderbuffer GLuint
 * \return void
 */
STATICINLINE void framebuffer_renderbuffer(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    group::renderbuffer_target    renderbuffertarget,
    u32                           renderbuffer,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferRenderbuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    glFramebufferRenderbuffer(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(renderbuffertarget),
        renderbuffer);
    detail::error_check("FramebufferRenderbuffer"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param target GLenum
 * \param attachment GLenum
 * \param textarget GLenum
 * \param texture GLuint
 * \param level GLint
 * \return void
 */
STATICINLINE void framebuffer_texture_1d(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    group::texture_target         textarget,
    u32                           texture,
    i32                           level,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTexture1D)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glFramebufferTexture1D(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level);
    detail::error_check("FramebufferTexture1D"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param target GLenum
 * \param attachment GLenum
 * \param textarget GLenum
 * \param texture GLuint
 * \param level GLint
 * \return void
 */
STATICINLINE void framebuffer_texture_2d(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    group::texture_target         textarget,
    u32                           texture,
    i32                           level,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTexture2D)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glFramebufferTexture2D(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level);
    detail::error_check("FramebufferTexture2D"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param target GLenum
 * \param attachment GLenum
 * \param textarget GLenum
 * \param texture GLuint
 * \param level GLint
 * \param zoffset GLint
 * \return void
 */
STATICINLINE void framebuffer_texture_3d(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    group::texture_target         textarget,
    u32                           texture,
    i32                           level,
    i32                           zoffset,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTexture3D)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glFramebufferTexture3D(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level,
        zoffset);
    detail::error_check("FramebufferTexture3D"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param target GLenum
 * \param attachment GLenum
 * \param texture GLuint
 * \param level GLint
 * \param layer GLint
 * \return void
 */
STATICINLINE void framebuffer_texture_layer(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    u32                           texture,
    i32                           level,
    i32                           layer,
    error_check                   check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTextureLayer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsTexture)
            glIsTexture(texture);
#endif
    }
    glFramebufferTextureLayer(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        texture,
        level,
        layer);
    detail::error_check("FramebufferTextureLayer"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param n GLsizei
 * \param framebuffers GLuint *
 * \return void
 */
STATICINLINE void gen_framebuffers(
    span_u32 framebuffers, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenFramebuffers)
    }
    glGenFramebuffers(
        framebuffers.size(),
        framebuffers.size() ? reinterpret_cast<GLuint*>(framebuffers.data())
                            : nullptr);
    detail::error_check("GenFramebuffers"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param n GLsizei
 * \param renderbuffers GLuint *
 * \return void
 */
STATICINLINE void gen_renderbuffers(
    span_u32 renderbuffers, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenRenderbuffers)
    }
    glGenRenderbuffers(
        renderbuffers.size(),
        renderbuffers.size() ? reinterpret_cast<GLuint*>(renderbuffers.data())
                             : nullptr);
    detail::error_check("GenRenderbuffers"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param target GLenum
 * \return void
 */
STATICINLINE void generate_mipmap(
    group::texture_target target, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenerateMipmap)
    }
    glGenerateMipmap(static_cast<GLenum>(target));
    detail::error_check("GenerateMipmap"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param target GLenum
 * \param attachment GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_framebuffer_attachment_parameter(
    group::framebuffer_target                    target,
    group::framebuffer_attachment                attachment,
    group::framebuffer_attachment_parameter_name pname,
    span_i32                                     params,
    error_check                                  check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFramebufferAttachmentParameteriv)
    }
    glGetFramebufferAttachmentParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetFramebufferAttachmentParameteriv"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_renderbuffer_parameter(
    group::renderbuffer_target         target,
    group::renderbuffer_parameter_name pname,
    span_i32                           params,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetRenderbufferParameteriv)
    }
    glGetRenderbufferParameteriv(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetRenderbufferParameteriv"sv, check_errors);
}

/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param framebuffer GLuint
 * \return Boolean
 */
STATICINLINE bool is_framebuffer(
    u32 framebuffer, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsFramebuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsFramebuffer)
            glIsFramebuffer(framebuffer);
#endif
    }
    auto out = glIsFramebuffer(framebuffer);
    detail::error_check("IsFramebuffer"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param renderbuffer GLuint
 * \return Boolean
 */
STATICINLINE bool is_renderbuffer(
    u32 renderbuffer, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsRenderbuffer)
#if(defined(GL_VERSION_2_0) || defined(GL_ES_VERSION_3_0)) && \
    !defined(GLEAM_USE_LINKED)
        if(glIsRenderbuffer)
            glIsRenderbuffer(renderbuffer);
#endif
    }
    auto out = glIsRenderbuffer(renderbuffer);
    detail::error_check("IsRenderbuffer"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

template<class size_2_i32>
requires(concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_ARB_framebuffer_object
 * \param target GLenum
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \return void
 */
STATICINLINE void renderbuffer_storage(
    group::renderbuffer_target target,
    group::internal_format     internalformat,
    size_2_i32 const&          width,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RenderbufferStorage)
    }
    glRenderbufferStorage(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("RenderbufferStorage"sv, check_errors);
}

template<class size_2_i32>
requires(concepts::size_2d<size_2_i32, i32>)
/*!
 * \brief Part of GL_ARB_framebuffer_object
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
    size_2_i32 const&          width,
    error_check                check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RenderbufferStorageMultisample)
    }
    glRenderbufferStorageMultisample(
        static_cast<GLenum>(target),
        samples,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("RenderbufferStorageMultisample"sv, check_errors);
}

} // namespace gl::arb::framebuffer_object
#endif // GL_ARB_framebuffer_object
namespace gl::arb::framebuffer_object {
constexpr auto name = "GL_ARB_framebuffer_object";
}
