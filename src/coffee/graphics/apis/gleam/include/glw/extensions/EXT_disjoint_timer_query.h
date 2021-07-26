#pragma once

#ifdef GL_EXT_disjoint_timer_query
namespace gl::ext::disjoint_timer_query {
namespace values {
constexpr libc_types::u32 query_counter_bits     = 0x8864;
constexpr libc_types::u32 current_query          = 0x8865;
constexpr libc_types::u32 query_result           = 0x8866;
constexpr libc_types::u32 query_result_available = 0x8867;
constexpr libc_types::u32 time_elapsed           = 0x88BF;
constexpr libc_types::u32 timestamp              = 0x8E28;
constexpr libc_types::u32 gpu_disjoint           = 0x8FBB;
} // namespace values
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
         std::decay_t<u32>>) STATICINLINE
    void delete_queries(span_const_u32 const& ids)
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
         std::decay_t<u32>>) STATICINLINE void gen_queries(span_u32 ids)
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

template<class span_i64>
requires(semantic::concepts::Span<span_i64>&& std::is_same_v<
         std::decay_t<typename span_i64::value_type>,
         std::decay_t<i64>>) STATICINLINE
    void get_integer64v(group::get_prop pname, span_i64 data)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetInteger64vEXT)
    }
    glGetInteger64vEXT(
        static_cast<GLenum>(pname),
        data.size() ? reinterpret_cast<GLint64*>(data.data()) : nullptr);
    detail::error_check("GetInteger64vEXT"sv);
}

template<class span_i64>
requires(semantic::concepts::Span<span_i64>&& std::is_same_v<
         std::decay_t<typename span_i64::value_type>,
         std::decay_t<i64>>) STATICINLINE
    void get_query_objecti64v(
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

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_query_objectiv(
        u32 id, group::query_object_parameter_name pname, span_i32 params)
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
    detail::error_check("GetQueryObjectivEXT"sv);
}

template<class span_u64>
requires(semantic::concepts::Span<span_u64>&& std::is_same_v<
         std::decay_t<typename span_u64::value_type>,
         std::decay_t<u64>>) STATICINLINE
    void get_query_objectui64v(
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

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void get_query_objectuiv(
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
         std::decay_t<i32>>) STATICINLINE
    void get_queryiv(
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

STATICINLINE void query_counter(u32 id, group::query_counter_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(QueryCounterEXT)
    }
    glQueryCounterEXT(id, static_cast<GLenum>(target));
    detail::error_check("QueryCounterEXT"sv);
}

} // namespace gl::ext::disjoint_timer_query
#endif // GL_EXT_disjoint_timer_query
namespace gl::ext::disjoint_timer_query {
constexpr auto name = "GL_EXT_disjoint_timer_query";
}
