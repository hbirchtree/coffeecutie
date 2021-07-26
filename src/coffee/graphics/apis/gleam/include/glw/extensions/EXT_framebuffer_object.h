#pragma once

#ifdef GL_EXT_framebuffer_object
#include "../enums/ErrorCode.h"
#include "../enums/FramebufferAttachmentParameterName.h"
#include "../enums/InternalFormat.h"
#include "../enums/InvalidateFramebufferAttachment.h"
#include "../enums/RenderbufferParameterName.h"
#include "../enums/SizedInternalFormat.h"
namespace gl::ext::framebuffer_object {
using gl::group::error_code;
using gl::group::framebuffer_attachment_parameter_name;
using gl::group::internal_format;
using gl::group::invalidate_framebuffer_attachment;
using gl::group::renderbuffer_parameter_name;
using gl::group::sized_internal_format;
namespace values {
constexpr libc_types::u32 max_renderbuffer_size                     = 0x84E8;
constexpr libc_types::u32 framebuffer_binding                       = 0x8CA6;
constexpr libc_types::u32 renderbuffer_binding                      = 0x8CA7;
constexpr libc_types::u32 framebuffer_complete                      = 0x8CD5;
constexpr libc_types::u32 framebuffer_incomplete_attachment         = 0x8CD6;
constexpr libc_types::u32 framebuffer_incomplete_missing_attachment = 0x8CD7;
constexpr libc_types::u32 framebuffer_incomplete_dimensions         = 0x8CD9;
constexpr libc_types::u32 framebuffer_incomplete_formats            = 0x8CDA;
constexpr libc_types::u32 framebuffer_incomplete_draw_buffer        = 0x8CDB;
constexpr libc_types::u32 framebuffer_incomplete_read_buffer        = 0x8CDC;
constexpr libc_types::u32 framebuffer_unsupported                   = 0x8CDD;
constexpr libc_types::u32 max_color_attachments                     = 0x8CDF;
constexpr libc_types::u32 framebuffer                               = 0x8D40;
constexpr libc_types::u32 renderbuffer                              = 0x8D41;
} // namespace values
STATICINLINE void bind_framebuffer(
    group::framebuffer_target target, u32 framebuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindFramebufferEXT)
        glIsFramebuffer(framebuffer);
    }
    glBindFramebufferEXT(static_cast<GLenum>(target), framebuffer);
    detail::error_check("BindFramebufferEXT"sv);
}

STATICINLINE void bind_renderbuffer(
    group::renderbuffer_target target, u32 renderbuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BindRenderbufferEXT)
        glIsRenderbuffer(renderbuffer);
    }
    glBindRenderbufferEXT(static_cast<GLenum>(target), renderbuffer);
    detail::error_check("BindRenderbufferEXT"sv);
}

STATICINLINE GLenum check_framebuffer_status(group::framebuffer_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CheckFramebufferStatusEXT)
    }
    auto out = glCheckFramebufferStatusEXT(static_cast<GLenum>(target));
    detail::error_check("CheckFramebufferStatusEXT"sv);
    return out;
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void delete_framebuffers(span_const_u32 const& framebuffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteFramebuffersEXT)
    }
    glDeleteFramebuffersEXT(
        framebuffers.size(),
        framebuffers.size()
            ? reinterpret_cast<const GLuint*>(framebuffers.data())
            : nullptr);
    detail::error_check("DeleteFramebuffersEXT"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void delete_renderbuffers(span_const_u32 const& renderbuffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteRenderbuffersEXT)
    }
    glDeleteRenderbuffersEXT(
        renderbuffers.size(),
        renderbuffers.size()
            ? reinterpret_cast<const GLuint*>(renderbuffers.data())
            : nullptr);
    detail::error_check("DeleteRenderbuffersEXT"sv);
}

STATICINLINE void framebuffer_renderbuffer(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    group::renderbuffer_target    renderbuffertarget,
    u32                           renderbuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferRenderbufferEXT)
        glIsRenderbuffer(renderbuffer);
    }
    glFramebufferRenderbufferEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(renderbuffertarget),
        renderbuffer);
    detail::error_check("FramebufferRenderbufferEXT"sv);
}

STATICINLINE void framebuffer_texture_1d(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    group::texture_target         textarget,
    u32                           texture,
    i32                           level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTexture1DEXT)
        glIsTexture(texture);
    }
    glFramebufferTexture1DEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level);
    detail::error_check("FramebufferTexture1DEXT"sv);
}

STATICINLINE void framebuffer_texture_2d(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    group::texture_target         textarget,
    u32                           texture,
    i32                           level)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTexture2DEXT)
        glIsTexture(texture);
    }
    glFramebufferTexture2DEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level);
    detail::error_check("FramebufferTexture2DEXT"sv);
}

STATICINLINE void framebuffer_texture_3d(
    group::framebuffer_target     target,
    group::framebuffer_attachment attachment,
    group::texture_target         textarget,
    u32                           texture,
    i32                           level,
    i32                           zoffset)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(FramebufferTexture3DEXT)
        glIsTexture(texture);
    }
    glFramebufferTexture3DEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(textarget),
        texture,
        level,
        zoffset);
    detail::error_check("FramebufferTexture3DEXT"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void gen_framebuffers(span_u32 framebuffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenFramebuffersEXT)
    }
    glGenFramebuffersEXT(
        framebuffers.size(),
        framebuffers.size() ? reinterpret_cast<GLuint*>(framebuffers.data())
                            : nullptr);
    detail::error_check("GenFramebuffersEXT"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void gen_renderbuffers(span_u32 renderbuffers)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenRenderbuffersEXT)
    }
    glGenRenderbuffersEXT(
        renderbuffers.size(),
        renderbuffers.size() ? reinterpret_cast<GLuint*>(renderbuffers.data())
                             : nullptr);
    detail::error_check("GenRenderbuffersEXT"sv);
}

STATICINLINE void generate_mipmap(group::texture_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenerateMipmapEXT)
    }
    glGenerateMipmapEXT(static_cast<GLenum>(target));
    detail::error_check("GenerateMipmapEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_framebuffer_attachment_parameter(
        group::framebuffer_target                    target,
        group::framebuffer_attachment                attachment,
        group::framebuffer_attachment_parameter_name pname,
        span_i32                                     params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetFramebufferAttachmentParameterivEXT)
    }
    glGetFramebufferAttachmentParameterivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(attachment),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetFramebufferAttachmentParameterivEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_renderbuffer_parameter(
        group::renderbuffer_target         target,
        group::renderbuffer_parameter_name pname,
        span_i32                           params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetRenderbufferParameterivEXT)
    }
    glGetRenderbufferParameterivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetRenderbufferParameterivEXT"sv);
}

STATICINLINE GLboolean is_framebuffer(u32 framebuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsFramebufferEXT)
        glIsFramebuffer(framebuffer);
    }
    auto out = glIsFramebufferEXT(framebuffer);
    detail::error_check("IsFramebufferEXT"sv);
    return out;
}

STATICINLINE GLboolean is_renderbuffer(u32 renderbuffer)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsRenderbufferEXT)
        glIsRenderbuffer(renderbuffer);
    }
    auto out = glIsRenderbufferEXT(renderbuffer);
    detail::error_check("IsRenderbufferEXT"sv);
    return out;
}

template<class size_2_i32>
requires(semantic::concepts::Size2D<size_2_i32, i32>) STATICINLINE
    void renderbuffer_storage(
        group::renderbuffer_target target,
        group::internal_format     internalformat,
        size_2_i32 const&          width)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(RenderbufferStorageEXT)
    }
    glRenderbufferStorageEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalformat),
        width[0],
        width[1]);
    detail::error_check("RenderbufferStorageEXT"sv);
}

} // namespace gl::ext::framebuffer_object
#endif // GL_EXT_framebuffer_object
namespace gl::ext::framebuffer_object {
constexpr auto name = "GL_EXT_framebuffer_object";
}
