#pragma once

#ifdef GL_EXT_timer_query
namespace gl::ext::timer_query {
namespace values {
constexpr libc_types::u32 time_elapsed = 0x88BF;
} // namespace values
template<class span_i64>
requires(semantic::concepts::Span<span_i64>&& std::is_same_v<
         std::decay_t<typename span_i64::value_type>,
         std::decay_t<i64>>)
    /*!
     * \brief Part of GL_EXT_timer_query
     * \param id GLuint
     * \param pname GLenum
     * \param params GLint64 *
     * \return void
     */
    STATICINLINE void get_query_objecti64v(
        u32 id, group::query_object_parameter_name pname, span_i64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryObjecti64vEXT)
    }
    glGetQueryObjecti64vEXT(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint64*>(params.data()) : nullptr);
    detail::error_check("GetQueryObjecti64vEXT"sv);
}

template<class span_u64>
requires(semantic::concepts::Span<span_u64>&& std::is_same_v<
         std::decay_t<typename span_u64::value_type>,
         std::decay_t<u64>>)
    /*!
     * \brief Part of GL_EXT_timer_query
     * \param id GLuint
     * \param pname GLenum
     * \param params GLuint64 *
     * \return void
     */
    STATICINLINE void get_query_objectui64v(
        u32 id, group::query_object_parameter_name pname, span_u64 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryObjectui64vEXT)
    }
    glGetQueryObjectui64vEXT(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint64*>(params.data()) : nullptr);
    detail::error_check("GetQueryObjectui64vEXT"sv);
}

} // namespace gl::ext::timer_query
#endif // GL_EXT_timer_query
namespace gl::ext::timer_query {
constexpr auto name = "GL_EXT_timer_query";
}
