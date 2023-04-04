#pragma once

#ifdef GL_ARB_occlusion_query
namespace gl::arb::occlusion_query {
namespace values {
constexpr libc_types::u32 query_counter_bits     = 0x8864;
constexpr libc_types::u32 current_query          = 0x8865;
constexpr libc_types::u32 query_result           = 0x8866;
constexpr libc_types::u32 query_result_available = 0x8867;
constexpr libc_types::u32 samples_passed         = 0x8914;
} // namespace values
/*!
 * \brief Part of GL_ARB_occlusion_query
 * \param target GLenum
 * \param id GLuint
 * \return void
 */
STATICINLINE void begin_query(group::query_target target, u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginQueryARB)
    }
    glBeginQueryARB(static_cast<GLenum>(target), id);
    detail::error_check("BeginQueryARB"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_ARB_occlusion_query
     * \param n GLsizei
     * \param ids const GLuint *
     * \return void
     */
    STATICINLINE void delete_queries(span_const_u32 const& ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteQueriesARB)
    }
    glDeleteQueriesARB(
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr);
    detail::error_check("DeleteQueriesARB"sv);
}

/*!
 * \brief Part of GL_ARB_occlusion_query
 * \param target GLenum
 * \return void
 */
STATICINLINE void end_query(group::query_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndQueryARB)
    }
    glEndQueryARB(static_cast<GLenum>(target));
    detail::error_check("EndQueryARB"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_ARB_occlusion_query
     * \param n GLsizei
     * \param ids GLuint *
     * \return void
     */
    STATICINLINE void gen_queries(span_u32 ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenQueriesARB)
    }
    glGenQueriesARB(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("GenQueriesARB"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_ARB_occlusion_query
     * \param id GLuint
     * \param pname GLenum
     * \param params GLint *
     * \return void
     */
    STATICINLINE void get_query_objectiv(
        u32 id, group::query_object_parameter_name pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryObjectivARB)
    }
    glGetQueryObjectivARB(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetQueryObjectivARB"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>)
    /*!
     * \brief Part of GL_ARB_occlusion_query
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
        GLW_FPTR_CHECK(GetQueryObjectuivARB)
    }
    glGetQueryObjectuivARB(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetQueryObjectuivARB"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>)
    /*!
     * \brief Part of GL_ARB_occlusion_query
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
        GLW_FPTR_CHECK(GetQueryivARB)
    }
    glGetQueryivARB(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetQueryivARB"sv);
}

/*!
 * \brief Part of GL_ARB_occlusion_query
 * \param id GLuint
 * \return Boolean
 */
STATICINLINE bool is_query(u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsQueryARB)
    }
    auto out = glIsQueryARB(id);
    detail::error_check("IsQueryARB"sv);
    return out == GL_TRUE ? true : false;
}

} // namespace gl::arb::occlusion_query
#endif // GL_ARB_occlusion_query
namespace gl::arb::occlusion_query {
constexpr auto name = "GL_ARB_occlusion_query";
}
