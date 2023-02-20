#pragma once

#ifdef GL_EXT_occlusion_query_boolean
namespace gl::ext::occlusion_query_boolean {
namespace values {
constexpr libc_types::u32 current_query                   = 0x8865;
constexpr libc_types::u32 query_result                    = 0x8866;
constexpr libc_types::u32 query_result_available          = 0x8867;
constexpr libc_types::u32 any_samples_passed              = 0x8C2F;
constexpr libc_types::u32 any_samples_passed_conservative = 0x8D6A;
} // namespace values
/*!
 * \brief Part of GL_EXT_occlusion_query_boolean
 * \param target GLenum
 * \param id GLuint
 * \return void
 */
STATICINLINE void begin_query(group::query_target target, u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginQueryEXT)
    }
    glBeginQueryEXT(static_cast<GLenum>(target), id);
    detail::error_check("BeginQueryEXT"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_EXT_occlusion_query_boolean
     * \param n GLsizei
     * \param ids const GLuint *
     * \return void
     */
    STATICINLINE void delete_queries(span_const_u32 const& ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteQueriesEXT)
    }
    glDeleteQueriesEXT(
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr);
    detail::error_check("DeleteQueriesEXT"sv);
}

/*!
 * \brief Part of GL_EXT_occlusion_query_boolean
 * \param target GLenum
 * \return void
 */
STATICINLINE void end_query(group::query_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndQueryEXT)
    }
    glEndQueryEXT(static_cast<GLenum>(target));
    detail::error_check("EndQueryEXT"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_EXT_occlusion_query_boolean
     * \param n GLsizei
     * \param ids GLuint *
     * \return void
     */
    STATICINLINE void gen_queries(span_u32 ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenQueriesEXT)
    }
    glGenQueriesEXT(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("GenQueriesEXT"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_EXT_occlusion_query_boolean
     * \param id GLuint
     * \param pname GLenum
     * \param params GLuint *
     * \return void
     */
    STATICINLINE void get_query_objectuiv(
        u32 id, group::query_object_parameter_name pname, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryObjectuivEXT)
    }
    glGetQueryObjectuivEXT(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetQueryObjectuivEXT"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_EXT_occlusion_query_boolean
     * \param target GLenum
     * \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_queryiv(
        group::query_target         target,
        group::query_parameter_name pname,
        span_i32                    params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryivEXT)
    }
    glGetQueryivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetQueryivEXT"sv);
}

/*!
 * \brief Part of GL_EXT_occlusion_query_boolean
 * \param id GLuint
 * \return Boolean
 */
STATICINLINE GLboolean is_query(u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsQueryEXT)
    }
    auto out = glIsQueryEXT(id);
    detail::error_check("IsQueryEXT"sv);
    return out;
}

} // namespace gl::ext::occlusion_query_boolean
#endif // GL_EXT_occlusion_query_boolean
namespace gl::ext::occlusion_query_boolean {
constexpr auto name = "GL_EXT_occlusion_query_boolean";
}
