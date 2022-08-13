#pragma once

#ifdef GL_NV_occlusion_query
#include "../enums/OcclusionQueryParameterNameNV.h"
namespace gl::nv::occlusion_query {
using gl::group::occlusion_query_parameter_name_nv;
namespace values {
constexpr libc_types::u32 pixel_counter_bits         = 0x8864;
constexpr libc_types::u32 current_occlusion_query_id = 0x8865;
} // namespace values
STATICINLINE void begin_occlusion_query(u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(BeginOcclusionQueryNV)
    }
    glBeginOcclusionQueryNV(id);
    detail::error_check("BeginOcclusionQueryNV"sv);
}

template<class span_const_u32>
requires(semantic::concepts::Span<span_const_u32>&& std::is_same_v<
         std::decay_t<typename span_const_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void delete_occlusion_queries(span_const_u32 const& ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(DeleteOcclusionQueriesNV)
    }
    glDeleteOcclusionQueriesNV(
        ids.size(),
        ids.size() ? reinterpret_cast<const GLuint*>(ids.data()) : nullptr);
    detail::error_check("DeleteOcclusionQueriesNV"sv);
}

STATICINLINE void end_occlusion_query()
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(EndOcclusionQueryNV)
    }
    glEndOcclusionQueryNV();
    detail::error_check("EndOcclusionQueryNV"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void gen_occlusion_queries(span_u32 ids)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GenOcclusionQueriesNV)
    }
    glGenOcclusionQueriesNV(
        ids.size(),
        ids.size() ? reinterpret_cast<GLuint*>(ids.data()) : nullptr);
    detail::error_check("GenOcclusionQueriesNV"sv);
}

template<class span_i32>
requires(semantic::concepts::Span<span_i32>&& std::is_same_v<
         std::decay_t<typename span_i32::value_type>,
         std::decay_t<i32>>) STATICINLINE
    void get_occlusion_queryiv(
        u32 id, group::occlusion_query_parameter_name_nv pname, span_i32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetOcclusionQueryivNV)
    }
    glGetOcclusionQueryivNV(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLint*>(params.data()) : nullptr);
    detail::error_check("GetOcclusionQueryivNV"sv);
}

template<class span_u32>
requires(semantic::concepts::Span<span_u32>&& std::is_same_v<
         std::decay_t<typename span_u32::value_type>,
         std::decay_t<u32>>) STATICINLINE
    void get_occlusion_queryuiv(
        u32 id, group::occlusion_query_parameter_name_nv pname, span_u32 params)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(GetOcclusionQueryuivNV)
    }
    glGetOcclusionQueryuivNV(
        id,
        static_cast<GLenum>(pname),
        params.size() ? reinterpret_cast<GLuint*>(params.data()) : nullptr);
    detail::error_check("GetOcclusionQueryuivNV"sv);
}

STATICINLINE GLboolean is_occlusion_query(u32 id)
{
    using namespace std::string_view_literals;
    if constexpr(compile_info::debug_mode)
    {
        GLW_FPTR_CHECK(IsOcclusionQueryNV)
    }
    auto out = glIsOcclusionQueryNV(id);
    detail::error_check("IsOcclusionQueryNV"sv);
    return out;
}

} // namespace gl::nv::occlusion_query
#endif // GL_NV_occlusion_query
namespace gl::nv::occlusion_query {
constexpr auto name = "GL_NV_occlusion_query";
}