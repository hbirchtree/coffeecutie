#pragma once

#ifdef GL_ARB_timer_query
#include "../enums/GetPName.h"
#include "../enums/QueryCounterTarget.h"
#include "../enums/QueryTarget.h"
namespace gl::arb::timer_query {
using gl::group::get_prop;
using gl::group::query_counter_target;
using gl::group::query_target;
namespace values {
} // namespace values
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
        GLW_FPTR_CHECK(GetQueryObjecti64v)
    }
    glGetQueryObjecti64v(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint64*>(params.data()) : nullptr);
    detail::error_check("GetQueryObjecti64v"sv);
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
        GLW_FPTR_CHECK(GetQueryObjectui64v)
    }
    glGetQueryObjectui64v(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint64*>(params.data()) : nullptr);
    detail::error_check("GetQueryObjectui64v"sv);
}

STATICINLINE void query_counter(u32 id, group::query_counter_target target)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(QueryCounter)
    }
    glQueryCounter(id, static_cast<GLenum>(target));
    detail::error_check("QueryCounter"sv);
}

} // namespace gl::arb::timer_query
#endif // GL_ARB_timer_query
namespace gl::arb::timer_query {
constexpr auto name = "GL_ARB_timer_query";
}