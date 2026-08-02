#pragma once

#ifdef GL_EXT_color_subtable
namespace gl::ext::color_subtable {
namespace values {
} // namespace values

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_color_subtable
 * \param target GLenum
 * \param start GLsizei
 * \param count GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param data const void *
 * \return void
 */
STATICINLINE void color_sub_table(
    group::color_table_target target,
    i32                       start,
    i32                       count,
    group::pixel_format       format,
    group::pixel_type         type,
    span_const_void const&    data,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ColorSubTableEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glColorSubTableEXT,
        gsl::span<const char>(
            reinterpret_cast<const char*>(data.data()), data.size_bytes()),
        target,
        start,
        count,
        format,
        type);
#endif
    glColorSubTableEXT(
        static_cast<GLenum>(target),
        start,
        count,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<const void*>(data.data()) : nullptr);
    detail::error_check("ColorSubTableEXT"sv, check_errors);
}

template<class vec_2_i32>
requires(concepts::vector<vec_2_i32, i32, 2>)
/*!
 * \brief Part of GL_EXT_color_subtable
 * \param target GLenum
 * \param start GLsizei
 * \param x GLint
 * \param y GLint
 * \param width GLsizei
 * \return void
 */
STATICINLINE void copy_color_sub_table(
    group::color_table_target target,
    i32                       start,
    vec_2_i32 const&          x,
    i32                       width,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(CopyColorSubTableEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glCopyColorSubTableEXT, target, start, x, width);
#endif
    glCopyColorSubTableEXT(
        static_cast<GLenum>(target), start, x[0], x[1], width);
    detail::error_check("CopyColorSubTableEXT"sv, check_errors);
}

} // namespace gl::ext::color_subtable
#endif // GL_EXT_color_subtable
namespace gl::ext::color_subtable {
constexpr auto name = "GL_EXT_color_subtable";
}
