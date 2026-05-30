#pragma once

#ifdef GL_EXT_occlusion_query_boolean
namespace gl::ext::occlusion_query_boolean {
namespace values {
constexpr u32 current_query                   = 0x8865;
constexpr u32 query_result                    = 0x8866;
constexpr u32 query_result_available          = 0x8867;
constexpr u32 any_samples_passed              = 0x8C2F;
constexpr u32 any_samples_passed_conservative = 0x8D6A;
} // namespace values

/*!
 * \brief Part of GL_EXT_occlusion_query_boolean
 * \param target GLenum
 * \param id GLuint
 * \return void
 */
STATICINLINE void begin_query(
    group::query_target target,
    u32                 id,
    error_check         check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginQueryEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glBeginQueryEXT, target, id);
#endif
    glBeginQueryEXT(static_cast<GLenum>(target), id);
    detail::error_check("BeginQueryEXT"sv, check_errors);
}

template<class span_const_u32>
requires(
    concepts::span<span_const_u32> &&
    std::is_same_v<
        std::decay_t<typename span_const_u32::value_type>,
        std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_occlusion_query_boolean
 * \param n GLsizei
 * \param ids const GLuint *
 * \return void
 */
STATICINLINE void delete_queries(
    span_const_u32 const& ids, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteQueriesEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glDeleteQueriesEXT,
        gsl::span<const char>(
            reinterpret_cast<const char*>(ids.data()), ids.size_bytes()));
#endif
    glDeleteQueriesEXT(
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr);
    detail::error_check("DeleteQueriesEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_occlusion_query_boolean
 * \param target GLenum
 * \return void
 */
STATICINLINE void end_query(
    group::query_target target, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndQueryEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glEndQueryEXT, target);
#endif
    glEndQueryEXT(static_cast<GLenum>(target));
    detail::error_check("EndQueryEXT"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_occlusion_query_boolean
 * \param n GLsizei
 * \param ids GLuint *
 * \return void
 */
STATICINLINE void gen_queries(
    span_u32 ids, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenQueriesEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGenQueriesEXT,
        gsl::span<char>(reinterpret_cast<char*>(ids.data()), ids.size_bytes()));
#endif
    glGenQueriesEXT(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("GenQueriesEXT"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
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
    u32                                id,
    group::query_object_parameter_name pname,
    span_u32                           params,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryObjectuivEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetQueryObjectuivEXT,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        id,
        pname);
#endif
    glGetQueryObjectuivEXT(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetQueryObjectuivEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
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
    span_i32                    params,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryivEXT)
    }
#ifdef GLW_FPTR_TRACE_DATA
    GLW_FPTR_TRACE_DATA(
        glGetQueryivEXT,
        gsl::span<char>(
            reinterpret_cast<char*>(params.data()), params.size_bytes()),
        target,
        pname);
#endif
    glGetQueryivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetQueryivEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_occlusion_query_boolean
 * \param id GLuint
 * \return GLboolean
 */
STATICINLINE GLboolean
is_query(u32 id, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsQueryEXT)
    }
#ifdef GLW_FPTR_TRACE
    GLW_FPTR_TRACE(glIsQueryEXT, id);
#endif
    auto out = glIsQueryEXT(id);
    detail::error_check("IsQueryEXT"sv, check_errors);
    return out;
}

} // namespace gl::ext::occlusion_query_boolean
#endif // GL_EXT_occlusion_query_boolean
namespace gl::ext::occlusion_query_boolean {
constexpr auto name = "GL_EXT_occlusion_query_boolean";
}
