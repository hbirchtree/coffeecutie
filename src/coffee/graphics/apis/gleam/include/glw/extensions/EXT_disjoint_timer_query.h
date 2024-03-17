#pragma once

#ifdef GL_EXT_disjoint_timer_query
#include "../enums/GetPName.h"
#include "../enums/QueryCounterTarget.h"

namespace gl::ext::disjoint_timer_query {
using gl::group::get_prop;
using gl::group::query_counter_target;

namespace values {
constexpr u32 query_counter_bits     = 0x8864;
constexpr u32 current_query          = 0x8865;
constexpr u32 query_result           = 0x8866;
constexpr u32 query_result_available = 0x8867;
constexpr u32 time_elapsed           = 0x88BF;
constexpr u32 gpu_disjoint           = 0x8FBB;
} // namespace values

/*!
 * \brief Part of GL_EXT_disjoint_timer_query
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
 * \brief Part of GL_EXT_disjoint_timer_query
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
    glDeleteQueriesEXT(
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr);
    detail::error_check("DeleteQueriesEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_disjoint_timer_query
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
    glEndQueryEXT(static_cast<GLenum>(target));
    detail::error_check("EndQueryEXT"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_disjoint_timer_query
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
    glGenQueriesEXT(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("GenQueriesEXT"sv, check_errors);
}

template<class span_i64>
requires(
    concepts::span<span_i64> && std::is_same_v<
                                    std::decay_t<typename span_i64::value_type>,
                                    std::decay_t<i64>>)
/*!
 * \brief Part of GL_EXT_disjoint_timer_query
 * \param pname GLenum
 * \param data GLint64 *
 * \return void
 */
STATICINLINE void get_integer64v(
    group::get_prop pname,
    span_i64        data,
    error_check     check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInteger64vEXT)
    }
    glGetInteger64vEXT(
        static_cast<GLenum>(pname),
        data.size() ? reinterpret_cast<GLint64*>(data.data()) : nullptr);
    detail::error_check("GetInteger64vEXT"sv, check_errors);
}

template<class span_i64>
requires(
    concepts::span<span_i64> && std::is_same_v<
                                    std::decay_t<typename span_i64::value_type>,
                                    std::decay_t<i64>>)
/*!
 * \brief Part of GL_EXT_disjoint_timer_query
 * \param id GLuint
 * \param pname GLenum
 * \param params GLint64 *
 * \return void
 */
STATICINLINE void get_query_objecti64v(
    u32                                id,
    group::query_object_parameter_name pname,
    span_i64                           params,
    error_check                        check_errors = error_check::on)
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
    detail::error_check("GetQueryObjecti64vEXT"sv, check_errors);
}

template<class span_i32>
requires(
    concepts::span<span_i32> && std::is_same_v<
                                    std::decay_t<typename span_i32::value_type>,
                                    std::decay_t<i32>>)
/*!
 * \brief Part of GL_EXT_disjoint_timer_query
 * \param id GLuint
 * \param pname GLenum
 * \param params GLint *
 * \return void
 */
STATICINLINE void get_query_objectiv(
    u32                                id,
    group::query_object_parameter_name pname,
    span_i32                           params,
    error_check                        check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetQueryObjectivEXT)
    }
    glGetQueryObjectivEXT(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetQueryObjectivEXT"sv, check_errors);
}

template<class span_u64>
requires(
    concepts::span<span_u64> && std::is_same_v<
                                    std::decay_t<typename span_u64::value_type>,
                                    std::decay_t<u64>>)
/*!
 * \brief Part of GL_EXT_disjoint_timer_query
 * \param id GLuint
 * \param pname GLenum
 * \param params GLuint64 *
 * \return void
 */
STATICINLINE void get_query_objectui64v(
    u32                                id,
    group::query_object_parameter_name pname,
    span_u64                           params,
    error_check                        check_errors = error_check::on)
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
    detail::error_check("GetQueryObjectui64vEXT"sv, check_errors);
}

template<class span_u32>
requires(
    concepts::span<span_u32> && std::is_same_v<
                                    std::decay_t<typename span_u32::value_type>,
                                    std::decay_t<u32>>)
/*!
 * \brief Part of GL_EXT_disjoint_timer_query
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
 * \brief Part of GL_EXT_disjoint_timer_query
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
    glGetQueryivEXT(
        static_cast<GLenum>(target),
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetQueryivEXT"sv, check_errors);
}

/*!
 * \brief Part of GL_EXT_disjoint_timer_query
 * \param id GLuint
 * \return Boolean
 */
STATICINLINE bool is_query(u32 id, error_check check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsQueryEXT)
    }
    auto out = glIsQueryEXT(id);
    detail::error_check("IsQueryEXT"sv, check_errors);
    return out == GL_TRUE ? true : false;
}

/*!
 * \brief Part of GL_EXT_disjoint_timer_query
 * \param id GLuint
 * \param target GLenum
 * \return void
 */
STATICINLINE void query_counter(
    u32                         id,
    group::query_counter_target target,
    error_check                 check_errors = error_check::on)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(QueryCounterEXT)
    }
    glQueryCounterEXT(id, static_cast<GLenum>(target));
    detail::error_check("QueryCounterEXT"sv, check_errors);
}

} // namespace gl::ext::disjoint_timer_query
#endif // GL_EXT_disjoint_timer_query
namespace gl::ext::disjoint_timer_query {
constexpr auto name = "GL_EXT_disjoint_timer_query";
}
