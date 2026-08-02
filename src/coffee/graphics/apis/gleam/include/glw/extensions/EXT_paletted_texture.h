#pragma once

#ifdef GL_EXT_paletted_texture
namespace gl::ext::paletted_texture {
namespace values {
constexpr u32 color_index1       = 0x80E2;
constexpr u32 color_index2       = 0x80E3;
constexpr u32 color_index4       = 0x80E4;
constexpr u32 color_index8       = 0x80E5;
constexpr u32 color_index12      = 0x80E6;
constexpr u32 color_index16      = 0x80E7;
constexpr u32 texture_index_size = 0x80ED;
} // namespace values

template<class span_const_void>
requires(concepts::span<span_const_void>)
/*!
 * \brief Part of GL_EXT_paletted_texture
 * \param target GLenum
 * \param internalFormat GLenum
 * \param width GLsizei
 * \param format GLenum
 * \param type GLenum
 * \param table const void *
 * \return void
 */
STATICINLINE void color_table(
    group::color_table_target target,
    group::internal_format    internalFormat,
    i32                       width,
    group::pixel_format       format,
    group::pixel_type         type,
    span_const_void const&    table,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(ColorTableEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glColorTableEXT,
        gsl::span<const char>(
            reinterpret_cast<const char*>(table.data()), table.size_bytes()),
        target,
        internalFormat,
        width,
        format,
        type);
#endif
    glColorTableEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(internalFormat),
        width,
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        table.size() ? reinterpret_cast<const void*>(table.data()) : nullptr);
    detail::error_check("ColorTableEXT"sv, check_errors);
}

template<class span_void>
requires(concepts::span<span_void>)
/*!
 * \brief Part of GL_EXT_paletted_texture
 * \param target GLenum
 * \param format GLenum
 * \param type GLenum
 * \param data void *
 * \return void
 */
STATICINLINE void get_color_table(
    group::color_table_target target,
    group::pixel_format       format,
    group::pixel_type         type,
    span_void                 data,
    error_check               check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetColorTableEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetColorTableEXT,
        gsl::span<char>(
            reinterpret_cast<char*>(data.data()), data.size_bytes()),
        target,
        format,
        type);
#endif
    glGetColorTableEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(format),
        static_cast<GLenum>(type),
        data.size() ? reinterpret_cast<void*>(data.data()) : nullptr);
    detail::error_check("GetColorTableEXT"sv, check_errors);
}

template<class span_f32>
requires(
    concepts::span<span_f32> && std::is_same_v<
                                    std::decay_t<typename span_f32::value_type>,
                                    std::decay_t<f32>>)
/*!
 * \brief Part of GL_EXT_paletted_texture
 * \param target GLenum
 * \param pname GLenum
 * \param params GLfloat *
 * \return void
 */
STATICINLINE void get_color_table_parameter(
    group::color_table_target         target,
    group::color_table_parameter_prop pname,
    span_f32                          params,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetColorTableParameterfvEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetColorTableParameterfvEXT,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetColorTableParameterfvEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLfloat*>(params.data()) : nullptr);
    detail::error_check("GetColorTableParameterfvEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_paletted_texture
 * \param target GLenum
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_color_table_parameter(
    group::color_table_target         target,
    group::color_table_parameter_prop pname,
    span_i32                          params,
    error_check                       check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(::gl::impl::debugging_enabled)
    {
        GLW_FPTR_CHECK(GetColorTableParameterivEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetColorTableParameterivEXT,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetColorTableParameterivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetColorTableParameterivEXT"sv, check_errors);
}

} // namespace gl::ext::paletted_texture
#endif // GL_EXT_paletted_texture
namespace gl::ext::paletted_texture {
constexpr auto name = "GL_EXT_paletted_texture";
}
