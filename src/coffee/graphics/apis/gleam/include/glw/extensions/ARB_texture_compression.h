#pragma once

#ifdef GL_ARB_texture_compression
#include "../enums/HintTarget.h"

namespace gl::arb::texture_compression {
using gl::group::hint_target;

namespace values {
constexpr u32 compressed_alpha               = 0x84E9;
constexpr u32 compressed_luminance           = 0x84EA;
constexpr u32 compressed_luminance_alpha     = 0x84EB;
constexpr u32 compressed_intensity           = 0x84EC;
constexpr u32 compressed_rgb                 = 0x84ED;
constexpr u32 compressed_rgba                = 0x84EE;
constexpr u32 texture_compressed_image_size  = 0x86A0;
constexpr u32 texture_compressed             = 0x86A1;
constexpr u32 num_compressed_texture_formats = 0x86A2;
constexpr u32 compressed_texture_formats     = 0x86A3;
} // namespace values

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_ARB_texture_compression
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param width GLsizei
 * \param border GLint
 * \param imageSize GLsizei
 * \param data const void *
 * \return void
 */
STATICINLINE void compressed_tex_image_1d(
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    i32                    width,
    i32                    border,
    span_const_void const& data,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexImage1DARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glCompressedTexImage1DARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(data.data()), data.size_bytes()),
        target,
        level,
        internalformat,
        width,
        border);
#endif
    glCompressedTexImage1DARB(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width,
        border,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexImage1DARB"sv, check_errors);
}

template<class size_2_i32, class span_const_void>
requires(concepts::size_2d<size_2_i32, i32> && concepts::span<span_const_void>)
/*!
 * \brief Part of GL_ARB_texture_compression
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param border GLint
 * \param imageSize GLsizei
 * \param data const void *
 * \return void
 */
STATICINLINE void compressed_tex_image_2d(
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    size_2_i32 const&      width,
    i32                    border,
    span_const_void const& data,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexImage2DARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glCompressedTexImage2DARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(data.data()), data.size_bytes()),
        target,
        level,
        internalformat,
        width,
        border);
#endif
    glCompressedTexImage2DARB(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        border,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexImage2DARB"sv, check_errors);
}

template<class size_3_i32, class span_const_void>
requires(concepts::size_2d<size_3_i32, i32> && concepts::span<span_const_void>)
/*!
 * \brief Part of GL_ARB_texture_compression
 * \param target GLenum
 * \param level GLint
 * \param internalformat GLenum
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param border GLint
 * \param imageSize GLsizei
 * \param data const void *
 * \return void
 */
STATICINLINE void compressed_tex_image_3d(
    group::texture_target  target,
    i32                    level,
    group::internal_format internalformat,
    size_3_i32 const&      width,
    i32                    border,
    span_const_void const& data,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexImage3DARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glCompressedTexImage3DARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(data.data()), data.size_bytes()),
        target,
        level,
        internalformat,
        width,
        border);
#endif
    glCompressedTexImage3DARB(
        static_cast<GLenum>(target),
        level,
        static_cast<GLenum>(internalformat),
        width[0],
        width[1],
        width[2],
        border,
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexImage3DARB"sv, check_errors);
}

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_ARB_texture_compression
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param width GLsizei
 * \param format GLenum
 * \param imageSize GLsizei
 * \param data const void *
 * \return void
 */
STATICINLINE void compressed_tex_sub_image_1d(
    group::texture_target  target,
    i32                    level,
    i32                    xoffset,
    i32                    width,
    group::internal_format format,
    span_const_void const& data,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexSubImage1DARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glCompressedTexSubImage1DARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(data.data()), data.size_bytes()),
        target,
        level,
        xoffset,
        width,
        format);
#endif
    glCompressedTexSubImage1DARB(
        static_cast<GLenum>(target),
        level,
        xoffset,
        width,
        static_cast<GLenum>(format),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexSubImage1DARB"sv, check_errors);
}

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32> &&
    concepts::span<span_const_void>)
/*!
 * \brief Part of GL_ARB_texture_compression
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param format GLenum
 * \param imageSize GLsizei
 * \param data const void *
 * \return void
 */
STATICINLINE void compressed_tex_sub_image_2d(
    group::texture_target  target,
    i32                    level,
    vec_2_i32 const&       xoffset,
    size_2_i32 const&      width,
    group::internal_format format,
    span_const_void const& data,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexSubImage2DARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glCompressedTexSubImage2DARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(data.data()), data.size_bytes()),
        target,
        level,
        xoffset,
        width,
        format);
#endif
    glCompressedTexSubImage2DARB(
        static_cast<GLenum>(target),
        level,
        xoffset[0],
        xoffset[1],
        width[0],
        width[1],
        static_cast<GLenum>(format),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexSubImage2DARB"sv, check_errors);
}

template<class size_3_i32, class span_const_void, class vec_3_i32>
requires(
    concepts::vector<vec_3_i32, i32, 3> && concepts::size_2d<size_3_i32, i32> &&
    concepts::span<span_const_void>)
/*!
 * \brief Part of GL_ARB_texture_compression
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param zoffset GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param depth GLsizei
 * \param format GLenum
 * \param imageSize GLsizei
 * \param data const void *
 * \return void
 */
STATICINLINE void compressed_tex_sub_image_3d(
    group::texture_target  target,
    i32                    level,
    vec_3_i32 const&       xoffset,
    size_3_i32 const&      width,
    group::internal_format format,
    span_const_void const& data,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(CompressedTexSubImage3DARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glCompressedTexSubImage3DARB,
        gsl::span<const char>(
            reinterpret_cast<const char*>(data.data()), data.size_bytes()),
        target,
        level,
        xoffset,
        width,
        format);
#endif
    glCompressedTexSubImage3DARB(
        static_cast<GLenum>(target),
        level,
        xoffset[0],
        xoffset[1],
        xoffset[2],
        width[0],
        width[1],
        width[2],
        static_cast<GLenum>(format),
        data.size() *
            sizeof(typename std::decay_t<span_const_void const&>::value_type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("CompressedTexSubImage3DARB"sv, check_errors);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_ARB_texture_compression
 * \param target GLenum
 * \param level GLint
 * \param img void *
 * \return void
 */
STATICINLINE void get_compressed_tex_image(
    group::texture_target target,
    i32                   level,
    span_void             img,
    error_check           check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetCompressedTexImageARB)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetCompressedTexImageARB,
        gsl::span<char>(reinterpret_cast<char*>(img.data()), img.size_bytes()),
        target,
        level);
#endif
    glGetCompressedTexImageARB(
        static_cast<GLenum>(target),
        level,
        img.size() ? reinterpret_cast<void*>(img.data()) : nullptr);
    detail::error_check("GetCompressedTexImageARB"sv, check_errors);
}

} // namespace gl::arb::texture_compression
#endif // GL_ARB_texture_compression
namespace gl::arb::texture_compression {
constexpr auto name = "GL_ARB_texture_compression";
}
