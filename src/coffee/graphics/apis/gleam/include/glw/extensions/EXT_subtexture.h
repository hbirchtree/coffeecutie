#pragma once

#ifdef GL_EXT_subtexture
namespace gl::ext::subtexture {
namespace values {
} // namespace values

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_subtexture
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param width GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void tex_sub_image_1d(
    group::texture_target  target,
    i32                    level,
    i32                    xoffset,
    i32                    width,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(TexSubImage1DEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glTexSubImage1DEXT,
        gsl::span<const char>(
            reinterpret_cast<const char*>(pixels.data()), pixels.size_bytes()),
        target,
        level,
        xoffset,
        width,
        format,
        type);
#endif
    glTexSubImage1DEXT(
        static_cast<GLenum>(target),
        level,
        xoffset,
        width,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexSubImage1DEXT"sv, check_errors);
}

template<class size_2_i32, class span_const_void, class vec_2_i32>
requires(
    concepts::vector<vec_2_i32, i32, 2> && concepts::size_2d<size_2_i32, i32> &&
    concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_subtexture
 * \param target GLenum
 * \param level GLint
 * \param xoffset GLint
 * \param yoffset GLint
 * \param width GLsizei
 * \param height GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param pixels const void *
 * \return void
 */
STATICINLINE void tex_sub_image_2d(
    group::texture_target  target,
    i32                    level,
    vec_2_i32 const&       xoffset,
    size_2_i32 const&      width,
    group::pixel_format    format,
    group::pixel_type      type,
    span_const_void const& pixels,
    error_check            check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(TexSubImage2DEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glTexSubImage2DEXT,
        gsl::span<const char>(
            reinterpret_cast<const char*>(pixels.data()), pixels.size_bytes()),
        target,
        level,
        xoffset,
        width,
        format,
        type);
#endif
    glTexSubImage2DEXT(
        static_cast<GLenum>(target),
        level,
        xoffset[0],
        xoffset[1],
        width[0],
        width[1],
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        pixels.size() ? reinterpret_cast<const void*>(pixels.data()) : nullptr);
    detail::error_check("TexSubImage2DEXT"sv, check_errors);
}

} // namespace gl::ext::subtexture
#endif // GL_EXT_subtexture
namespace gl::ext::subtexture {
constexpr auto name = "GL_EXT_subtexture";
}
